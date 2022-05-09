#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <io.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

// 获取文件夹内所有文件的绝对路径
void getFileNames(string path, vector<string>& files);

// 定义结构体，存储文件名与文件大小
typedef struct fileMessage
{
	char fileName[50];
	long int filesize;
}fileMessage;

int main()
{
  // tcp连接部分
  WORD wVersionRequested;
  WSADATA wsaData;
  wVersionRequested = MAKEWORD(2, 2);
  WSAStartup( wVersionRequested, &wsaData );
 
  unsigned int sockSrv = socket(AF_INET, SOCK_STREAM, 0);
  SOCKADDR_IN addrSrv;
 
  addrSrv.sin_family = AF_INET; 
  addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
  addrSrv.sin_port = htons(8888); 
  
  bind(sockSrv,(SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

  while(1)
  {
  listen(sockSrv, 50);
   cout<< "listening" <<endl;

  SOCKADDR_IN addrClient;
  int len = sizeof(SOCKADDR);
  unsigned int sockConn = accept(sockSrv,(SOCKADDR*)&addrClient, &len);
  cout<< "accepted" <<endl;

  while(1)
  {
    char status[4]={0};
    recv(sockConn, status, sizeof(status), 0);
    printf("recv status from client: %s\n", status);
    int choice = atoi(status);
    // 返回file文件夹下的所有文件路径
    if(choice == 1)
    {
      vector<string> fileNames;
      string path("file");
      getFileNames(path, fileNames);
      char testname[500]={0};
      for (const auto &ph : fileNames) {
        strcat(testname ,ph.c_str());
        strcat(testname,";");
	    }
      // cout<<testname<<endl;
      send(sockConn, testname,sizeof(testname),0);
    }
    // 客户端上传文件
    else if(choice == 2)
    {
      fileMessage fileMsg;
      long int fileSize = 0;
      char filename[50] = {0};
      // 接收ftp client发送过来的文件长度与文件名 
      char sizeFileStr[20] = {0};
      recv(sockConn, (char*)&fileMsg, sizeof(fileMsg), 0);
      
      fileSize = fileMsg.filesize;
      // int fileSize = atoi(sizeFileStr);

      strcpy(filename, fileMsg.fileName);
    
      // 接收ftp client发送的文件并保存
      char recvBuf[1024*1024] = {0};  
      int recvTotalSize = 0;
      char file[50] = "file\\";
      strcat(file,filename);
      FILE *fp = fopen(file, "wb");
      while(recvTotalSize < fileSize)
      {
        int recvSize = recv(sockConn, recvBuf, sizeof(recvBuf), 0);
        recvTotalSize += recvSize;
        printf("received %f KB\n", recvTotalSize/(1024.0));
        fwrite(recvBuf, 1, recvSize, fp);
      }
      fclose(fp);
    
      if(recvTotalSize == fileSize)
      {printf("Done!\n");}
      else
      {printf("Error!");}
    }
    // 客户端从服务端下载文件
    else if(choice == 3)
    {
      fileMessage re_fileMsg;
      char re_filename[50] = {0};
      recv(sockConn,re_filename,sizeof(re_filename), 0);
      // cout<<re_filename<<endl;

      char re_file[50] = "file\\";
      strcat(re_file,re_filename);
      cout<<re_file<<endl;

      FILE *fp = fopen(re_file, "rb");
      fseek( fp, 0, SEEK_END );
      int totalSize =  ftell(fp);
      cout<<totalSize<<endl;
      fclose(fp);
      fp = fopen(re_file, "rb");
      int  readSize = 0;
      int  sendTotalSize = 0;
      char sendBuf[1024*1024] = {0};
      while((readSize = fread(sendBuf, 1, sizeof(sendBuf), fp)) > 0)
      {
        send(sockConn, sendBuf, readSize, 0);
        sendTotalSize += readSize;

        printf("sent %f KB\n", sendTotalSize/(1024.0));
      }
      fclose(fp);
      if(sendTotalSize == totalSize)
      {printf("Done!\n");}
      else
      {printf("Error!");}

    }
    // 退出
    else if(choice == 4)
    {break;}
    // if(strcmp,'3')
  }
  }
  closesocket(sockSrv);
  WSACleanup();
 
  // getchar();
  cout<<"Bye!"<<endl;
  
  return 0;
}

void getFileNames(string path, vector<string>& files)
{
	//文件句柄
	intptr_t hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFileNames(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}