// Created by JasonZYT on 02/03/2021
#include "pch.h"
#include "config.h"
#include "func.h"
#include "logger.h"

using namespace std;

namespace QuickBackupX
{
	Config::Config()
	{
		//this->getConfig();
	}

	bool Config::EditPermissionName(PerType per, string xuid, string name)
	{
		int i = 0;
		Json::Value jarr;
		switch (per)
		{
		case Config::PerType::Admin:
			jarr = this->cfgjv["Admin_Player"];
			break;
		case Config::PerType::Backup:
			jarr = this->cfgjv["Backup_Player"];
			break;
		case Config::PerType::Back:
			jarr = this->cfgjv["Back_Player"];
			break;
		default:
			return false;
			break;
		}
		int jarrs = jarr.size();
		while (i <= jarrs - 1)
		{
			if (jarr[i].empty()) continue;
			if (jarr[i]["Xuid"].asString() == xuid)
			{
				this->cfgjv[i]["Name"] = name;
				SWriteIntoFile(this->cfgjv, CONFIGFILE);
				return true;
			}
			i++;
		}
	}

	bool Config::PlayerIsAdmin(string name, string xuid)
	{
		map<string, string>::iterator iter = cfg->admins.begin();
		for (; iter != cfg->backup.end(); ++iter)
		{
			if (iter->first == name)
			{
				if (iter->second != xuid)
				{
					PRWARN(u8"��� " << name << " AdminȨ�޼��ʧ��: Xuid��ƥ��");
					L_WARNING(string("��� ") + name + " AdminȨ�޼��ʧ��: Xuid��ƥ��");
					return false;
				}
				else return true;
			}
			if (iter->second == xuid)
			{
				if (iter->first != name)
					cfg->EditPermissionName(Config::PerType::Admin, xuid, name);
				return true;
			}
		}
		PRWARN(u8"��� " << name << " AdminȨ�޼��ʧ��: ��Ȩ��");
		L_WARNING(string("��� ") + name + " AdminȨ�޼��ʧ��: ��Ȩ��");
		return false;
	}
	string Config::getBackupDir()
	{
		size_t pos = this->bop.find_last_of("/");
		if (pos == string::npos) pos = this->bop.find_last_of("\\");
		if (pos == string::npos)
		{
			PRERR(u8"BackupOutputPathֵ����!������\"/\"��\"\\\"��");
			L_ERROR("BackupOutputPathֵ����!������\"/\"��\"\\\"��");
			Sleep(3000);
			throw 109;
		}
		string dir = this->bop.substr(0, pos + 1);
		return filesystem::canonical(filesystem::path(dir)).string();
	}
	string Config::getBackupDisk()
	{
		string bdir = this->getBackupDir();
		return bdir.substr(0, 3);
	}

	bool Config::getJsonArray(Json::Value root)
	{
		int i = 0;
		Json::Value adminv = root["Admin_Player"];
		int admini = adminv.size();
		if (admini != 0)
		{
			i = 0;
			while (i <= adminv.size() - 1)
			{
				if (adminv[i].empty()) continue;
				this->admins.insert(
					pair<string, string>(
						adminv[i]["Name"].asString(),
						adminv[i]["Xuid"].asString()
					)
				);
				i++;
			}
		}
		//Json::Value backupc = root["Cloud_Backup_Player"];
		Json::Value backup = root["Backup_Player"];
		//Json::Value backc = root["Cloud_Back_Player"];
		Json::Value back = root["Back_Player"];
		//int backupci = backupc.size();
		int backupi = backup.size();
		//int backci = backc.size();
		int backi = back.size();
		/*if (backupci != 0)
		{
			i = 0;
			while (i <= backupc.size() - 1)
			{
				this->backupc.insert(
					pair<string, string>(
						backupc[i]["Name"].asString(), 
						backupc[i]["Xuid"].asString()
					)
				);
				i++;
			}
		}*/
		if (backupi != 0)
		{
			i = 0;
			while (i <= backup.size() - 1)
			{
				if (backup[i].empty()) continue;
				this->backup.insert(
					pair<string, string>(
						backup[i]["Name"].asString(),
						backup[i]["Xuid"].asString()
					)
				);
				i++;
			}
		}
		/*if (backci != 0)
		{
			i = 0;
			while (i <= backc.size() - 1)
			{
				this->backc.insert(
					pair<string, string>(
						backc[i]["Name"].asString(),
						backc[i]["Xuid"].asString()
					)
				);
				i++;
			}
		}*/
		if (backi != 0)
		{
			i = 0;
			while (i <= back.size() - 1)
			{
				if (back[i].empty()) continue;
				this->back.insert(
					pair<string, string>(
						back[i]["Name"].asString(),
						back[i]["Xuid"].asString()
					)
				);
				i++;
			}
		}
		return true;
	}
	bool Config::getConfig()
	{
		this->cfgjv = getJSON(CONFIGFILE);
		Json::Value root = this->cfgjv;
		Json::Value example;
		example[0]["Name"] = "Steve";
		example[1]["Name"] = "Alex";
		example[1]["Xuid"] = "1145141919810";
		if (!root.isMember("Debug")            || !root["Debug"]           .isBool())   root["Debug"] = true;
		if (!root.isMember("Allow_CB")         || !root["Debug"]           .isBool())   root["Allow_CB"] = false;
		if (!root.isMember("BackupOutputPath") || !root["BackupOutputPath"].isString()) root["BackupOutputPath"] = "./backup/%Y-%m-%d-%H-%M.zip";
		if (!root.isMember("EULA")             || !root["EULA"]            .isBool())   root["EULA"] = false;
		if (!root.isMember("Admin_Player")     || !root["Admin_Player"]    .isArray())  root["Admin_Player"] = example;
		if (!root.isMember("Backup_Player")    || !root["Backup_Player"]   .isArray())  root["Backup_Player"] = example;
		if (!root.isMember("Back_Player")      || !root["Back_Player"]     .isArray())  root["Back_Player"] = example;
		SWriteIntoFile(root, CONFIGFILE);
		if (root.empty() || !root.isObject())
		{
			L_ERROR("����Ϊ�ջ����ò��Ƕ�������");
			throw 110;
			return false;
		}
		if (!root["EULA"].asBool())
		{
			PRERR(u8"��δͬ��EULA,����" << CONFIGFILE << u8"�н�\"EULA\": false��Ϊ\"EULA\": true,���̼�������");
			Sleep(3000);
			exit(-1);
		}
		this->debug = root["Debug"].asBool();
		this->acb = root["Allow_CB"].asBool();
		this->bop = root["BackupOutputPath"].asString();
		this->eula = root["EULA"].asBool();
		this->getJsonArray(root);
		Debug{
			L_INFO("���ڶ�ȡ����: ");
			L_INFO(string("- Debug: ") + (this->debug ? "true" : "false"));
			L_INFO(string("- Allow_CB: ") + (this->acb ? "true" : "false"));
			L_INFO(string("- BackupOutputPath: ") + this->bop);
			L_INFO(string("- EULA: ") + (this->eula ? "true" : "false"));
			L_INFO(string("- Admin_Player(") + to_string(this->admins.size()) + ") ");// + this->cfgjv["Admin_Player"].asString());
			L_INFO(string("- Backup_Player(") + to_string(this->backup.size()) + ") ");// + this->cfgjv["Backup_Player"].asString());
			L_INFO(string("- Back_Player(") + to_string(this->back.size()) + ") ");// +this->cfgjv["Back_Player"].asString());
		}
		L_INFO("���ö�ȡ�ɹ�!");
		PR(u8"���ö�ȡ�ɹ�!");
		return true;
	}
}