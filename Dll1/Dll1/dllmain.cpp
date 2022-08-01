/*
 *  Copyright(r) 2020
 *
 *  ViralDB is an extension for Arma 3 Server that provides an interface to MySQL/MariaDB database.
 *
 *  Author: G-Virus
 *
 *  Return messages:
 *
 *      code:message - code means a type of the message.
 *
 *  Commands:
 *
 *      connect - connect to a MySQL/MariaDB database.
 *             args    [host, port, user, password, databaseName].
 *             returns error message or "success".
 *
 *      checkUser - obtains a user from the database
 *             args    [steamID, username].
 *             returns "valid" or reason why the user is unable to connect to the server.
 */

#include "pch.h"

#include "mysql.h"

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>


#ifndef _INC_STDLIB
#include <stdlib.h>
#endif // !_INC_STDLIB




__declspec (dllexport) void __stdcall  RVExtension(char* output, int outputSize, const char* function);
__declspec (dllexport) int  __stdcall  RVExtensionArgs(char* output, int outputSize, const char* function, const char** argv, int argc);
__declspec (dllexport) void __stdcall  RVExtensionVersion(char* output, int outputSize);




MYSQL* connection = NULL;
char     host[1024];
uint16_t port = 3306;
char     user[256];
char     password[256];
char     dbName[256];

typedef enum
{
    eUndefined = '0',
    eSystem = '1',
    eQuery = '2'
} EMsgClass;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void)hinstDLL;
    (void)lpvReserved;

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        mysql_library_init(0, NULL, NULL);

        break;
    }

    case DLL_PROCESS_DETACH:
    {
        if (connection)
            mysql_close(connection);

        mysql_library_end();

        break;
    }
    }

    return TRUE;
}

void reply(char* output, EMsgClass class1, const char* message)
{
    if (message && output)
    {
        output[0] = (char)class1;
        output[1] = ':';
        strcpy(&output[2], message);
    }
}

char assign(char* dst, const char* src)
{
    if (strlen(src) < sizeof(dst))
    {
        strcpy(dst, src);
        return 1;
    }

    return 0;
}

char strptime(const char* data, struct tm* timeinfo)
{
    if (timeinfo)
    {
        sscanf_s(data, "%4d-%2d-%2d %2d:%2d:%2d",
            &timeinfo->tm_year,
            &timeinfo->tm_mon,
            &timeinfo->tm_mday,
            &timeinfo->tm_hour,
            &timeinfo->tm_min,
            &timeinfo->tm_sec);

        return 1;
    }

    return 0;
}

char checkBanTime(const char* banTime)
{
    if (banTime)
    {
        struct tm timeinfo;

        if (strptime(banTime, &timeinfo))
        {
            time_t curTime;
            time_t userBanTime = mktime(&timeinfo);

            if (userBanTime != -1)
            {
                time(&curTime);
                return difftime(curTime, userBanTime) >= 0;
            }
        }
    }

    return 0;
}

char getUserFromDB(const char* steamID,
    char* output,
    char* name,
    char* tag,
    char* banTime)
{
    if (connection && steamID && name && tag && banTime && output)
    {
        MYSQL_STMT* stmt = mysql_stmt_init(connection);

        const char* query = "SELECT `username`, `tag`, `srvbantime` FROM `phpbb_users` WHERE `streamid`=?;";

        if (!mysql_stmt_prepare(stmt, query, strlen(query)))
        {
            char isOK = 0;
            const char* errorString = "\0";

            MYSQL_BIND params[1];

            memset(params, 0, sizeof(MYSQL_BIND));

            params[0].buffer_type = MYSQL_TYPE_STRING;
            params[0].buffer = (char*)steamID;
            params[0].buffer_length = strlen(steamID);
            params[0].length = 0;
            params[0].is_null = 0;

            if (!mysql_stmt_bind_param(stmt, params))
            {
                if (!mysql_stmt_execute(stmt))
                {

                    int columnNum = mysql_stmt_field_count(stmt);

                    if (columnNum > 0)
                    {
                        MYSQL_RES* metadata = mysql_stmt_result_metadata(stmt);
                        MYSQL_FIELD* fields = mysql_fetch_fields(metadata);
                        MYSQL_BIND* result = new MYSQL_BIND [columnNum];

                        memset(result, 0, sizeof(MYSQL_BIND) * columnNum);

                        
                        std::vector<char*> userData(columnNum);                      

                        std::vector<char> nulls(columnNum);

                        for (int i = 0; i < columnNum; ++i)
                        {


                            result[i].buffer_type = fields[i].type;
                            result[i].is_null = &nulls[i];
                            result[i].buffer = userData[i];
                            result[i].buffer_length = sizeof(userData[i]);
                        }

                        if (!mysql_stmt_bind_result(stmt, result))
                        {
                            for (;;)
                            {
                                int status = mysql_stmt_fetch(stmt);

                                if (status == 1)
                                {
                                    errorString = mysql_stmt_error(stmt);
                                    isOK = 0;
                                    break;
                                }
                                else if (status == MYSQL_NO_DATA)
                                {
                                    isOK = 1;

                                    if (!nulls[0])
                                        strcpy(name, userData[0]);

                                    if (!nulls[1])
                                        strcpy(tag, userData[1]);

                                    if (!nulls[2])
                                        strcpy(banTime, userData[2]);

                                    break;
                                }
                            }
                        }
                        else
                        {
                            errorString = mysql_stmt_error(stmt);
                        }

                        mysql_free_result(metadata);
                        delete[] result;
                    }
                }
                else
                {
                    errorString = mysql_stmt_error(stmt);
                }
            }
            else
            {
                errorString = mysql_stmt_error(stmt);
            }

            mysql_stmt_close(stmt);

            if (!isOK && strcmp(errorString, "\0") != 0)
                reply(output, eQuery, errorString);

            return isOK;
        }
        else
        {
            reply(output, eQuery, mysql_stmt_error(stmt));
        }
    }
    else
    {
        reply(output, eQuery, "uninitialized data");
    }

    return 0;
}

void RVExtension(char* output, int outputSize, const char* function)
{
    (void)outputSize;
    (void)function;
    reply(output, eSystem, "unrecognized command");
}

int RVExtensionArgs(char* output, int outputSize, const char* function, const char** argv, int argc)
{
    (void)outputSize;

    if (strcmp(function, "checkUser") == 0)
    {
        if (argc == 2)
        {
            char username[256];
            char tag[20];
            char banTime[256];

            memset(username, 0, sizeof(username));
            memset(tag, 0, sizeof(tag));
            memset(banTime, 0, sizeof(banTime));

            reply(output, eQuery, "not found");

            if (getUserFromDB(argv[0], output, username, tag, banTime))
            {
                char totalName[300];
                const char* name = NULL;

                if (strlen(tag) != 0)
                {
                    sprintf(totalName, "[%s]%s", tag, username);
                    name = totalName;
                }
                else
                {
                    name = username;
                }

                if (strcmp(name, argv[1]) == 0)
                {
                    if (banTime[0] == 0 || !checkBanTime(banTime))
                        reply(output, eQuery, "banned");
                    else
                        reply(output, eQuery, "valid");
                }
                else
                {
                    reply(output, eQuery, "wrong nickname");
                }
            }
        }
        else
        {
            reply(output, eQuery, "wrong number of arguments in \'checkUser\' command");
        }
    }
    else if (strcmp(function, "connect") == 0)
    {
        if (argc == 5)
        {
            for (int i = 0; i < argc; ++i)
            {
                char portBuf[5];
                char isOK = 0;

                switch (i)
                {
                case 0: isOK = assign(host, argv[0]); break;
                case 1: isOK = assign(portBuf, argv[1]); break;
                case 2: isOK = assign(user, argv[2]); break;
                case 3: isOK = assign(password, argv[3]); break;
                case 4: isOK = assign(dbName, argv[4]); break;
                }

                if (!isOK)
                {
                    char msg[30];
                    sprintf_s(msg, "argument %d is too long", i);
                    reply(output, eSystem, msg);
                }
                else if (i == 1)
                {
                    port = (uint16_t)atoi(portBuf);
                }
            }

            if (connection)
                mysql_close(connection);

            connection = mysql_init(NULL);

            if (mysql_real_connect(connection, host, user, password, dbName, port, NULL, 0))
            {
                reply(output, eSystem, "success");
            }
            else
            {
                char msg[1024];
                sprintf_s(msg, "failed to connect to MySQL: %s", mysql_error(connection));
                reply(output, eSystem, msg);
            }
        }
        else
        {
            reply(output, eSystem, "wrong number of arguments in \'connect\' command");
        }
    }

    return 0;
}

void RVExtensionVersion(char* output, int outputSize)
{
    (void)outputSize;
    strcpy(output, "0.1");
}
