#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <mariadb/mysql.h>

MYSQL* conn;
MYSQL_ROW sqlrow;

int main(int argc, char *argv[])
{
	int tring = 23;
	int echo = 24;
	int led_1 = 17;
	int start_time, end_time;
	float distance;

	char sql_cmd[200] = {0};
	char* host = "localhost";
	char* user = "iot";
	char* pass = "pwiot";
	char* dbname = "iotdb";
	int res;

	conn = mysql_init(NULL);
	
	if(wiringPiSetup() == -1 ) exit(1);


	pinMode(tring, OUTPUT);
	pinMode(echo, INPUT);

	if (!(mysql_real_connect(conn, host, user, pass, dbname, 0, NULL, 0)))
	{
		fprintf(stderr, "ERROR : %s[%d]\n", mysql_error(conn), mysql_errno(conn));
		exit(1);
	}
	else
		printf("Connection Successful!\n\n");



	while(1) {
		digitalWrite(tring, LOW);
		delay(500);
		digitalWrite(tring, HIGH);
		delayMicroseconds(10);
		digitalWrite(tring, LOW);
		while(digitalRead(echo) == 0);
		start_time = micros();
		while(digitalRead(echo) == 1);
		end_time = micros();
		distance = (end_time - start_time) / 29. / 2.;

		printf("distance %.2f cm\n", distance);

		sprintf(sql_cmd, "insert into sensor(date, time, height) values(now(),now(),%.2f)", distance);
		res = mysql_query(conn, sql_cmd);
		if (!res)
			printf("inserted %lu rows\n", (unsigned long)mysql_affected_rows(conn));
		else
			fprintf(stderr, "ERROR: %s[%d]\n", mysql_error(conn), mysql_errno(conn));

	}

	mysql_close(conn);

	return 0;





}
