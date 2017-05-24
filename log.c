#include "server.h"

void connection_log(client_info_t client_info){
	FILE *fp=fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "CONNECTION  client_addr %s   client_fd %d\n", ip, client_info.client_fd);
	fprintf(stderr, "CONNECTION  client_addr %s   client_fd %d\n", ip, client_info.client_fd);
	fclose(fp);
}

void okay_log(client_info_t client_info){
	FILE *fp=fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "SEND TO  client_addr %s   client_fd %d  message OKAY\n", ip, client_info.client_fd);
	fprintf(stderr, "SEND TO  client_addr %s   client_fd %d  message OKAY\n", ip, client_info.client_fd);
	fclose(fp);
}
void pong_log(client_info_t client_info){
	FILE *fp=fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "SEND TO  client_addr %s   client_fd %d  message PONG\n", ip, client_info.client_fd);
	fprintf(stderr, "SEND TO  client_addr %s   client_fd %d  message PONG\n", ip, client_info.client_fd);
	fclose(fp);
}
void error_log(client_info_t client_info, char *msg){
	FILE *fp=fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "SEND TO  client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fprintf(stderr, "SEND TO  client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fclose(fp);
}

void solu_log(client_info_t client_info, char *msg){
	FILE *fp=fopen("log.txt", "a");
	if(fp == NULL){
		printf("failed to open log\n");
	}
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "SEND TO  client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fprintf(stderr, "SEND TO  client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fclose(fp);
}

void receive_log(client_info_t client_info, char *msg) {
	FILE *fp = fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "FROM client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fprintf(stderr, "FROM client_addr %s   client_fd %d  message %s\n", ip, client_info.client_fd, msg);
	fclose(fp);
}

void disconnect_log(client_info_t client_info){
	FILE *fp=fopen("log.txt", "a");
	time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(fp, "Timestamp: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	char ip[20];
	inet_ntop(AF_INET, &(client_info.client_addr.sin_addr), ip, 20);
	fprintf(fp, "DISCONNECT  client_addr %s   client_fd %d\n", ip, client_info.client_fd);
	fprintf(stderr, "DISCONNECT  client_addr %s   client_fd %d\n", ip, client_info.client_fd);
	fclose(fp);
}
