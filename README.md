os-scheduler
=============
3-1 운영체제 팀프로젝트 - 스케줄러 구현 (SPN, LPN, RR)


# 서론
* 스케줄링의 필요성 : 응답 시간이나 처리량, 효율성을 증대 시키기 위해 처리기가 다음에 실행할 프로세스를 선택해야 한다.
* 스케줄링 알고리즘을 3개를 선별하여 각각의 알고리즘에 프로세스 5개의 도착 시간과 서비스 시간으로 종료 시간, 반환 시간, 정규화 된 반환시간을 측정한다. 
* Switch – case문으로 3가지의 스케줄링 알고리즘을 시연한다. 스케줄링 알고리즘은 함수별로 작성하였음.

# 스케줄러 소개
## SPN
FCFS 기반에서 가장 짧은 프로세스를 먼저 실행시키는 방법
* 서비스 시간이 짧은 프로세스가 비록 늦게 도착했더라도 긴 프로세스 보다 먼저 스케줄링 될 수 있다.

## LPN
FCFS 기반에서 가장 긴 프로세스를 먼저 실행시키는 방법
* 서비스 시간이 긴 프로세스가 비록 늦게 도착했더라도 짧은 프로세스 보다 먼저 스케줄링 될 수 있다.


## RR
FCFS 기반에서 짧은 프로세스가 피해보는 현상을 완화하는 방법
* 시간을 측정하면서 프로세스가 일정 시간(시간 할당량 : Time Quantum) 이상을 넘어가는 순간 실행을 선점 모드로써 강제로 프로세스를 중단한다.


# 결과
## SPN
<img src="https://user-images.githubusercontent.com/37360089/71074653-03402c80-21c6-11ea-9f18-9b415346ba1f.png"><img>
## LPN
<img src="https://user-images.githubusercontent.com/37360089/71074705-1a7f1a00-21c6-11ea-9965-ac91a97cf442.png"><img>
## RR
### time quantum=3
<img src="https://user-images.githubusercontent.com/37360089/71074738-236feb80-21c6-11ea-9538-9b73dfbeb450.png"><img>
### time quantum=4
<img src="https://user-images.githubusercontent.com/37360089/71074750-29fe6300-21c6-11ea-9252-c39def0a14f0.png"><img>
