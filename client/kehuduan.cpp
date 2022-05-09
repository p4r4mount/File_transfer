#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

// 定义结构体，存储文件名与文件大小
typedef struct fileMessage
{
	char fileName[50];
	long int filesize;
}fileMessage;

void showmenu();
void upload();
void comfort();

int main()
{
  // tcp连接部分
  WORD wVersionRequested;
  WSADATA wsaData;
  wVersionRequested = MAKEWORD(2, 2);
  
  WSAStartup( wVersionRequested, &wsaData );
 
  SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
 
  SOCKADDR_IN addrSrv;
  addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
  addrSrv.sin_family = AF_INET;
  addrSrv.sin_port = htons(8888);
  connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
  cout<<"connected"<<endl;
  
  // 调出主菜单
  showmenu();
  int choice;
  cin >> choice;
  cin.sync();
  
  
  // 进入主循环
  while (1)
  {
  char choice_send[4] = {0};
  itoa(choice,choice_send, 10);
  send(sockClient, choice_send, sizeof(choice_send), 0);
  // 浏览服务器上的文件
  if(choice == 1)
  {
    char allfilesname[500];
    recv(sockClient, allfilesname, sizeof(allfilesname), 0);
    // cout<<allfilesname<<endl;
    cout<<"Files on the server:"<<endl;
    // 字符串分割
    char *next_token = NULL;
    char *token;
    token = strtok_s(allfilesname,";",&next_token);

    while(token!=NULL)
    {
        cout<<token<<endl;
        token = strtok_s(NULL,";",&next_token);
    }
    getchar();
  }
  // 客户端上传文件
  else if (choice == 2)
  {
    fileMessage fileMsg;
    char filename[50] ={0};
    cout<<"please input upload file's name:"<<endl;
    cin.getline(filename,50);

    FILE *fp = fopen(filename, "rb");
    fseek( fp, 0, SEEK_END );
    int totalSize =  ftell(fp);
    fclose(fp);
  
    // ftp client发送文件长度及文件名
    char fileSizeStr[20] = {0};
    itoa(totalSize, fileSizeStr, 10);
    fileMsg.filesize = totalSize;
    strcpy(fileMsg.fileName,filename);

    // send(sockClient, fileSizeStr, strlen(fileSizeStr) + 1, 0);

    send(sockClient, (char*)&fileMsg, sizeof(fileMsg), 0);


    // ftp client
  
    // ftp client发送文件
    fp = fopen(filename, "rb");
    int  readSize = 0;
    int  sendTotalSize = 0;
    char sendBuf[1024*1024] = {0};
    while((readSize = fread(sendBuf, 1, sizeof(sendBuf), fp)) > 0)
    {
      send(sockClient, sendBuf, readSize, 0); 
      sendTotalSize += readSize;
  
      printf("sent %f KB\n", sendTotalSize/(1024.0));
    }
    fclose(fp);
  
    if(sendTotalSize == totalSize)
    {printf("Done!\n");}
    else
    {printf("Error!");}
    
    getchar();
  }
  // 客户端下载文件
  else if (choice == 3)
  {
    fileMessage rec_fileMsg;
    char rec_filename[50] ={0};
    cout<<"please input download file's name:"<<endl;
    cin.getline(rec_filename,50);

    send(sockClient, rec_filename, sizeof(rec_filename), 0);

    char recvBuf[1024*1024] = {0}; 
    int recvTotalSize = 0;
    FILE *fp = fopen(rec_filename, "wb");
    int fileSize = 16;
    while (recvTotalSize<fileSize)
    {
      int recvSize = recv(sockClient,recvBuf,sizeof(recvBuf), 0);
      recvTotalSize+=recvSize;
      printf("received %f KB\n", recvTotalSize/(1024.0));
      fwrite(recvBuf, 1, recvSize, fp);
    }
    fclose(fp);
    getchar();
  }
  // 退出
  else if (choice == 4)
  {
    cout<<"Bye!"<<endl;
    break;
  }
  else{
    cout<<"please input valid number!"<<endl;
  }
  showmenu();
  cin >> choice;
  cin.sync();
  }
  closesocket(sockClient);
  WSACleanup();
  
  // getchar();
  // printf("Bye!\n");
  return 0;
}

void showmenu()
{
    cout << "please enter 1, 2, 3,or 4:\n"
            "1) browse           2) upload\n"
            "3) download         4) quit\n";
}
