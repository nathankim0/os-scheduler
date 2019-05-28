#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
////////////////////////추가된 메세지입니다.
#define MAX_PROCESS   512
#define MAX_QUEUE   256
#define SORT_SER_TIME      0x00000001
#define SORT_REMAIN_TIME   0x00000002

typedef int(*comp)(const void*, const void*);
typedef struct process {
	int pid; //프로세스번호
	int ar_t; //도착시간
	int ser_t; //수행시간
	bool is_finished; //종료판단변수
	int remain_time; //수행후 남아있는 시간
	int waiting_time; //대기시간
	int end_time; //종료시간
	int put; //프로세스 실행 판단 변수(0or1)
} process;

process g_process[MAX_PROCESS];
process *prc_queue[MAX_QUEUE] = { NULL };
int q_head = 0;
int q_tail = -1;
int pr_cnt = -1;

//fcfs 정렬_순차정렬
int fcfs_sort(process *a, process *b)
{
	if (a->ar_t > b->ar_t)
		return 1;
	else if (a->ar_t < b->ar_t)
		return -1;
	else
		return 0;
}

//프로세스 종료 확인 함수
int is_all_finish()
{
	int i;
	for (i = 0; i <= pr_cnt; i++)
	{
		if (g_process[i].remain_time != 0)
			return 0;
	}
	return 1;
}

//프로세스를 큐 끝에 삽입
int prc_enqueue(process *prc)
{
	prc_queue[++q_tail] = prc;

	return 1;
}

//프로세스를 큐에서 제거
int prc_dequeue()
{
	prc_queue[q_head] = NULL;
	q_head++;
	if (q_head > q_tail)
	{
		return 0;
	}
	return 1;
}

//대기큐 실행시간 오름차순 정렬
int spn_queue_sort(int start, int end)
{
	int i, j, cnt;
	process *temp; //프로세스 임시 저장 변수
	for (i = start, cnt = 0; i <= end; i++, cnt++)
	{
		for (j = start + 1; j <= end - cnt; j++)
		{
			/* 이전 큐에 있는 프로세스 수행시간이 다음 큐에 있는 수행시간보다 길다면
			-> 두 프로세스의 순서를 바꾼다 (오름차순) */
			if (prc_queue[j - 1]->ser_t > prc_queue[j]->ser_t)
			{
				//프로세스 임시변수에 짧은 수행시간을 가진 (j번째) 프로세스 저장 
				temp = prc_queue[j];
				// j번째 프로세스에 수행시간이 긴 j-1 프로세스를 저장 
				prc_queue[j] = prc_queue[j - 1];
				//프로세스 저장 변수를 이용해 수행시간이 짧은 프로세스를 앞으로 순서를 바꾼다.
				prc_queue[j - 1] = temp;
			}
		}
	}
	return 1;
}


//대기큐 실행시간 내림차순 정렬
int lpn_queue_sort(int start, int end)
{
	int cnt = 0;
	process *temp; //프로세스 임시 저장 변수

	for (int i = start; i <= end; i++, cnt++)
	{
		for (int j = start + 1; j <= end - cnt; j++)
		{
			/* 이전 큐에 있는 프로세스 수행시간이 다음 큐에 있는 프로세스 수행시간보다 짧다면
			-> 두 프로세스의 순서를 바꾼다 (내림차순) */
			if (prc_queue[j - 1]->ser_t < prc_queue[j]->ser_t)
			{
				//프로세스 임시변수에 긴 수행시간을 가진 (j번째) 프로세스 저장 
				temp = prc_queue[j];
				// j번째 프로세스에 수행시간이 짧은 j-1 프로세스를 저장 
				prc_queue[j] = prc_queue[j - 1];
				//프로세스 저장 변수를 이용해 수행시간이 긴 프로세스를 앞으로 순서를 바꾼다.
				prc_queue[j - 1] = temp;
			}

		}
	}
	return 1;
}

//SPN 알고리즘
void SPN(int cnt)
{
	cnt--;
	pr_cnt = cnt;
	//파일 출력 정의
	FILE* file2 = fopen("output.txt", "w");
	//FCFS정렬 (순차정렬) 및 스케줄링
	qsort((void *)g_process, pr_cnt + 1, sizeof(process), (comp)fcfs_sort);

	int i, j;
	int wait_sum = g_process[0].ar_t;
	int average_wait = 0;
	double average_banhwan = 0;
	int queue_empty = 0;
	int current_pos = 0;

	for (i = 0; !is_all_finish(); i++)
	{
		//정렬된 첫 번째 프로세스의 도착시간 찾기
		if (i < g_process[0].ar_t) {
			continue;
		}
		//현재 시간과 동일한 도착시간 찾기
		for (j = 0; j <= pr_cnt; j++)
		{
			if (i == g_process[j].ar_t)
			{
				prc_enqueue(&g_process[j]); //동일한 도착시간을 가진 프로세스를 큐에 삽입한다.
				queue_empty = 0;
				spn_queue_sort(q_head + 1, q_tail);
			}
		}

		//큐가 비어있지 않으면 삽입
		if (queue_empty == 1) {
			continue;
		}

		//수행 후 남아있는 시간 줄임
		prc_queue[q_head]->remain_time--;

		//남아있는 수행시간이 없다면 프로세스 출력
		/* \nPID : %d, 도착시간 : %ds , 서비스시간 : %ds 종료시간 : %ds, 반환시간 : %ds, 정규화된 반환시간 : %.2fs\n\n */
		if (prc_queue[q_head]->remain_time == 0) {
			prc_queue[q_head]->is_finished = 1;
			printf("%d \t\t %ds\t\t  %ds\t\t  %ds\t\t %ds\t\t %.2fs \n\n", prc_queue[q_head]->pid, prc_queue[q_head]->ar_t, prc_queue[q_head]->ser_t, i + 1, (i + 1) - prc_queue[q_head]->ar_t, (double)(prc_queue[q_head]->waiting_time + prc_queue[q_head]->ser_t) / (double)prc_queue[q_head]->ser_t);
			fprintf(file2, "프로세스 id\t 도착시간\t 서비스 시간\t 종료 시간\t 반환 시간\t 정규화된 반환 시간\n");
			fprintf(file2, "%d \t\t %ds\t\t  %ds\t\t  %ds\t\t %ds\t\t %.2fs \n\n", prc_queue[q_head]->pid, prc_queue[q_head]->ar_t, prc_queue[q_head]->ser_t, i + 1, (i + 1) - prc_queue[q_head]->ar_t, (double)(prc_queue[q_head]->waiting_time + prc_queue[q_head]->ser_t) / (double)prc_queue[q_head]->ser_t);
			
			//대기시간 계산    
			for (j = q_head + 1; j <= q_tail; j++)
				prc_queue[j]->waiting_time++;

			// queue[0]
			if (prc_queue[q_head]->remain_time == 0) {
				prc_queue[q_head]->is_finished = 1;
				if (prc_dequeue() == 0)
					queue_empty = 1;
			}
		}
	}
	for (i = 0; i <= pr_cnt; i++)
	{
		average_wait += g_process[i].waiting_time;
	}
	printf("\n평균 대기 시간: %.2f\n", (float)average_wait / i);

	for (i = 0; i <= pr_cnt; i++)
	{
		/*printf("P%d : 대기:%d, 서비스:%d, 대기+서비스/서비스: %.2f \n", g_process[i].pid, g_process[i].waiting_time, g_process[i].ser_t, (double)(g_process[i].waiting_time + g_process[i].ser_t) / (double)g_process[i].ser_t);*/

		average_banhwan += (double)(g_process[i].waiting_time + g_process[i].ser_t) / (double)g_process[i].ser_t;
		//printf("반환시간더하기: %.2f\n", average_banhwan);
	}
	printf("\n평균 반환 시간: %.2f\n", (double)average_banhwan / (double)i);

	fprintf(file2, "\n평균 대기 시간: %.2f\n", (float)average_wait / i);
	fprintf(file2, "\n평균 반환 시간: %.2f\n", (double)average_banhwan / (double)i);

}

//LPN 알고리즘
void LPN(int cnt)
{
	cnt--;
	pr_cnt = cnt;
	FILE* file2 = fopen("output.txt", "w");
	//FCFS정렬 (순차정렬) 및 스케줄링
	qsort((void *)g_process, pr_cnt + 1, sizeof(process), (comp)fcfs_sort);

	int i, j;
	int wait_sum = g_process[0].ar_t; //대기시간 총합
	int average_wait = 0; //평균 대기시간
	double average_banhwan = 0;
	int queue_empty = 0; //큐 비어있는지 확인

	int current_pos = 0;
	for (i = 0; !is_all_finish(); i++)
	{
		//정렬된 첫 번째 프로세스의 도착시간 찾기
		if (i < g_process[0].ar_t) {
			continue;
		}

		//현재 시간과 동일한 도착시간 찾기
		for (j = 0; j <= pr_cnt; j++)
		{
			if (i == g_process[j].ar_t)
			{
				prc_enqueue(&g_process[j]); //동일한 도착시간을 가진 프로세스를 큐에 삽입한다.
				queue_empty = 0;
				lpn_queue_sort(q_head + 1, q_tail);
			}
		}

		//큐가 비어있지 않으면 삽입
		if (queue_empty == 1) {
			continue;
		}

		//수행 후 남아있는 시간 줄임
		prc_queue[q_head]->remain_time--;

		//대기시간 계산    
		for (j = q_head + 1; j <= q_tail; j++)
			prc_queue[j]->waiting_time++;


		//남아있는 수행시간이 없다면 프로세스 출력
		/* \nPID : %d, 도착시간 : %ds , 서비스시간 : %ds 종료시간 : %ds, 반환시간 : %ds, 정규화된 반환시간 : %.2fs\n\n */
		if (prc_queue[q_head]->remain_time == 0) {
			prc_queue[q_head]->is_finished = 1;
			printf("%d \t\t %ds\t\t  %ds\t\t  %ds\t\t %ds\t\t %.2fs \n\n", prc_queue[q_head]->pid, prc_queue[q_head]->ar_t, prc_queue[q_head]->ser_t, i + 1, (i + 1) - prc_queue[q_head]->ar_t, (double)(prc_queue[q_head]->waiting_time + prc_queue[q_head]->ser_t) / (double)prc_queue[q_head]->ser_t);
			fprintf(file2, "프로세스 id\t 도착시간\t 서비스 시간\t 종료 시간\t 반환 시간\t 정규화된 반환 시간\n");
			fprintf(file2, "%d \t\t %ds\t\t  %ds\t\t  %ds\t\t %ds\t\t %.2fs \n\n", prc_queue[q_head]->pid, prc_queue[q_head]->ar_t, prc_queue[q_head]->ser_t, i + 1, (i + 1) - prc_queue[q_head]->ar_t, (double)(prc_queue[q_head]->waiting_time + prc_queue[q_head]->ser_t) / (double)prc_queue[q_head]->ser_t);

			if (prc_dequeue() == 0)
				queue_empty = 1;
		}
	}
	for (i = 0; i <= pr_cnt; i++)
	{
		average_wait += g_process[i].waiting_time;
	}
	printf("\n평균 대기 시간: %.2f\n", (float)average_wait / i);

	for (i = 0; i <= pr_cnt; i++)
	{
		/*printf("P%d : 대기:%d, 서비스:%d, 대기+서비스/서비스: %.2f \n", g_process[i].pid, g_process[i].waiting_time, g_process[i].ser_t, (double)(g_process[i].waiting_time + g_process[i].ser_t) / (double)g_process[i].ser_t);*/

		average_banhwan += (double)(g_process[i].waiting_time + g_process[i].ser_t) / (double)g_process[i].ser_t;
		//printf("반환시간더하기: %.2f\n", average_banhwan);
	}
	printf("\n평균 반환 시간: %.2f\n", (double)average_banhwan / (double)i);

	fprintf(file2, "\n평균 대기 시간: %.2f\n", (float)average_wait / i);
	fprintf(file2, "\n평균 반환 시간: %.2f\n", (double)average_banhwan / (double)i);
}

//RR 알고리즘
void RR(int cnt, process s[]) {
	int tm;
	int time_q = 4, time = 0;
	int max_wait_index = 0; //가장 오래기다린 프로세스 Index
	int one_q = 1; //colck 인터럽트 발생 여부 판단 변수
	int temp_time = 0; //임시 시간 변수
	int exit = 0; //종료 프로세스 개수
	int average_wait = 0;
	double average_banhwan = 0;
				  //파일 출력 정의
	FILE* file2 = fopen("output.txt", "w");

	while (1) {
		for (int i = 0; i < cnt; i++) {
			//현재 시간 = 도착시간 -> 프로세스 실행판단 변수 1
			if (g_process[i].ar_t == time) g_process[i].put = 1;

			//프로세스 생성 && 클럭인터럽트 발생 
			if (g_process[i].put == 1 && one_q == 1) {

				//ser_t != 0 && 가장 오래 기다린 프로세스의 대기시간보다 큰 대기시간을 가질때
				if (g_process[i].ser_t != 0 && g_process[i].waiting_time >= g_process[max_wait_index].waiting_time) {
					//max_wait_index의 인덱스 값 저장
					max_wait_index = i;
				}
			}
		}
		//실행 후 대기시간 0으로 초기화
		g_process[max_wait_index].waiting_time = 0;

		//실행 했으므로 수행 시간 -1
		g_process[max_wait_index].ser_t--;
		//time quantum =4일 때 종료를 위해 +1
		temp_time++;

		for (int i = 0; i < cnt; i++) {
			//대기중인 프로세스&& 현재 비 실행 프로세스일 경우
			if (i != max_wait_index && g_process[i].put == 1) {
				//대기시간 +1
				g_process[i].waiting_time++;
			}
		}
			//가장 오래 기다린 프로세스의 수행시간이 0 && 4초 이상 실행 시 중단
			if (g_process[max_wait_index].ser_t != 0 && temp_time != time_q) {
				one_q = 0;
			}
			else {
				//수행시간이 0일 때 프로세스 실행 종료
				if (g_process[max_wait_index].ser_t == 0) {
					g_process[tm].put = 0;

					//프로세스 정보 출력
					printf("%d\t\t %ds\t\t  %ds\t\t  %ds\t\t %ds\t\t %.2fs \n\n", s[tm].pid, s[tm].ar_t, s[tm].ser_t,
						time + 1, time + 1 - s[tm].ar_t, (time + 1 - s[tm].ar_t) / (double)s[tm].ser_t);
					fprintf(file2, "프로세스 id\t 도착시간\t 서비스 시간\t 종료 시간\t 반환 시간\t 정규화된 반환 시간\n");
					fprintf(file2, "%d\t\t %ds\t\t  %ds\t\t  %ds\t\t %ds\t\t %.2fs \n\n", s[tm].pid, s[tm].ar_t, s[tm].ser_t,
						time + 1, time + 1 - s[tm].ar_t, (time + 1 - s[tm].ar_t) / (double)s[tm].ser_t);

					exit++;

					average_wait += (time + 1 - s[tm].ar_t - s[tm].ser_t);
					average_banhwan += ((time + 1 - s[tm].ar_t) / (double)s[tm].ser_t);
				}
				//q=4와 비교할 시간 값 0으로 초기화
				temp_time = 0;
				//인터럽트 발생 여부 변수 1로 변경
				one_q = 1;
			}

			//시간 +1   
			time++;
			//프로세스 개수 = 종료된 프로세스 개수 ->while문 종료
			if (exit == cnt) break;
		}
		printf("\n평균 대기시간: %d\n", average_wait / cnt);
		printf("\n평균 반환시간: %.2f\n", average_banhwan / cnt);
		fprintf(file2, "\n평균 대기시간: %d\n", average_wait / cnt);
		fprintf(file2, "\n평균 반환시간: %.2f\n", average_banhwan / cnt);
}
int main()
{
	FILE* file = NULL;
	char c[MAX_PROCESS];
	char* a, *b = NULL;

	process copy_sys[MAX_QUEUE] = { 0, };

	int count = 0, i;
	int max_wait_index = 0, one_q = 1, exit = 0;
	int u;

	memset(g_process, 0, sizeof(g_process));

	//파일 읽어오기
	file = fopen("input.txt", "r");
	if (file == NULL) {
		printf("파일이 존재하지 않습니다.");
		return -1;
	}
	while (fgets(c, MAX_PROCESS, file) != NULL) {
		if (c[0] == '\n') continue;

		a = strtok(c, ",");
		if (*a == '0') {
			break;
		}
		g_process[count].pid = atoi(a);

		a = strtok(NULL, ",");
		g_process[count].ar_t = atoi(a);
		a = strtok(NULL, ",");
		g_process[count].remain_time = g_process[count].ser_t = atoi(a);
		g_process[count].waiting_time = 0;
		count++;

	}
	fclose(file); //파일 닫기

	printf("3조\n\n");
	printf("input.txt\n");
	//input.txt 파일 정보 출력
	for (i = 0; i < count; i++) {
		copy_sys[i] = g_process[i];
		//PID, 도착시간, 수행시간
		printf("%d %d %d \n", g_process[i].pid, g_process[i].ar_t, g_process[i].ser_t);
	}
	puts("===============================================");

	printf("1. SPN \n2. RR\n3. LPN\n4. 종료\n");
	printf("입력하세요: ");

	scanf("%d", &u);
	switch (u) {
	case 1:
		puts("*****************************************************");
		printf("\n<SPN 스케줄링>\n\n");
		printf("프로세스 id\t 도착시간\t 서비스 시간\t 종료 시간\t 반환 시간\t 정규화된 반환 시간\n");
		printf("=======================================================================================================\n");
		SPN(count);
		printf("=======================================================================================================\n");
		break;
	case 2:
		puts("*****************************************************");
		printf("\n<RR 스케줄링>\n\n");
		printf("프로세스 id\t 도착시간\t 서비스 시간\t 종료 시간\t 반환 시간\t 정규화된 반환 시간\n");
		printf("=======================================================================================================\n");
		RR(count, copy_sys);
		printf("=======================================================================================================\n");
		break;
	case 3:
		puts("*****************************************************");
		printf("\n<LRN 스케줄링>\n\n");
		printf("프로세스 id\t 도착시간\t 서비스 시간\t 종료 시간\t 반환 시간\t 정규화된 반환 시간\n");
		printf("=======================================================================================================\n");
		LPN(count);
		printf("=======================================================================================================\n");
		break;
	case 4:
		return 0;
	default: printf("잘못 입력했음"); break;
	}
}