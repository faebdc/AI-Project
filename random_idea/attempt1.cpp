#include<cstdio>
#include<cstdlib>
#include<ctime>
#include<cstring>
#include<iostream>
#include<cmath>
using namespace std;
const int N = 6;
const int M = 10000;
const int Type_num = 3;
const int act_num = 2;

double alpha=1e-4,Gamma=0.9,eps=0.1;

int Type = 1;
bool reach_N;

double QValue[N+1][N+1];
double qReward[N+1][N+1];

int subgoal;

int rs(int x)
{
	int res=rand();
	res%=x;
	res<<=10;
	res%=x;
	res+=rand();
	res%=x;
	res<<=10;
	res+=rand();
	res%=x;
	res++;
	return res;
}

void Init()
{
	memset(QValue,0,sizeof QValue);
	memset(qReward,0,sizeof qReward);
	//subgoal=1;
}

double getReward(int state,int action)
{
	int nstate;
	double rew;
	if(action==1)
		nstate=state-1;
	else
		nstate=state+1;
	if(nstate<1)
		nstate=1;
	if(nstate>N)
		nstate=N;
	if(nstate==1)
	{
		if(reach_N)
			rew=1000;
		else
			rew=1;
	}

	else
		rew=0;
	return rew;
}

int getNextState(int state,int action, bool in_set_up = false)
{
	int nstate;
	double rew;
	if(action==1)
		nstate=state-1;
	else
	{
		int temp = rs(2);
		if(temp == 2)
			nstate = state + 1;
		else
			nstate = state - 1;
	}
	if(nstate<1)
		nstate=1;
	if(nstate>N)
		nstate=N;
	if(nstate==1)
	{
		if(reach_N)
			rew=1000;
		else
			rew=1;
	}
	else
		rew=0;
	if(nstate==N && (in_set_up == false))
		reach_N=1;
	return nstate;
}

int getOptQA(int state)
{
	double val=-1e10;
	int i;
	int act=0;
	for(i=1;i<=act_num;i++)
	{
		if(QValue[state][i]>val)
		{
			val=QValue[state][i];
			act=i;
		}
	}
	return act;
}

double getQReward(int state,int action)
{
	//int nstate=getNextState(state,action);
	return qReward[state][action];
}

int getAction(int state)
{
	int action;
	//if(Type==1)//eps-greedy
	{
		if(rs(10000)<=10000*eps)
			action=rs(act_num);
		else
			action=getOptQA(state);
	}
	int nxtstate=getNextState(state,action);
	QValue[state][action]+=alpha*(getQReward(state,action)+Gamma*QValue[nxtstate][getOptQA(nxtstate)]-QValue[state][action]);
	return action;
}

void SetQReward(int state)
{
	int i,j;
	if(Type==1)
	{
		for(i=1;i<=N;i++)
		{
			for(j=1;j<=act_num;j++)
			{
				qReward[i][j]=getReward(i,j);
			}
		}
	}
	if(Type==2)	//random
	{
		for(i=1;i<=N;i++)
		{
			for(j=1;j<=act_num;j++)
			{
				qReward[i][j]=getReward(i,j);
			}
		}
		while(subgoal==state)
			subgoal=rs(N);
			//subgoal=rs(act_num);
		for(i=1;i<=N;i++)
		{
			for(j=1;j<=act_num;j++)
			{
				int k;
				//qReward[i][j] = 0;
				for(k=1;k<=10;k++)
				{
					double now=fabs(getNextState(i,j, true)-subgoal);
					qReward[i][j]+=10000*(N-now);
				}
			}
		}
	}
	if(Type==3)	//most unexplored
	{
		for(i=1;i<=N;i++)
		{
			for(j=1;j<=act_num;j++)
			{
				qReward[i][j]=getReward(i,j);
			}
		}
		if(subgoal==state)
		{
			if(N-state>state-1)
				subgoal=N;
			else
				subgoal=1;
		}
		for(i=1;i<=N;i++)
		{
			for(j=1;j<=act_num;j++)
			{
				int k;
				//qReward[i][j] = 0;
				for(k=1;k<=10;k++)
				{
					double now=fabs(getNextState(i,j, true)-subgoal);
					qReward[i][j]+=10000*(N-now);
				}
			}
		}
	}
	
}

int main()
{
	srand(time(0));
	int i;
	for(Type=1;Type<=Type_num;Type++)
	{
		Init();
		int count[N+1];
		cout<<"Type:"<<Type<<"\n";
		for(int t = 0; t < 100; t++)
		{
			memset(count, 0,sizeof(count));
			eps=(99-t)*0.003;
			for(int k = 0; k < 100000; k++)
			{
				bool vis[N+1];
				memset(vis,0,sizeof vis);
				subgoal = 2;
				reach_N=0;
				int state=2;
				double reward=0;
				for(i=1;i<=M;i++)
				{
					SetQReward(state);
					int action=getAction(state);
					//action=2;
					reward+=getReward(state,action);
					state=getNextState(state,action);
					if(!vis[state])
					{
						count[state]++;
						vis[state]=1;
					}
					if(state == 1)
					{
						break;
					}
				}
			}
			for(int k = 1; k < N+1; k++)
			{
				cout<<count[k]<<" ";
			}
			cout<<"\n";
		}
		//printf("Type==%d\nScore==%g\n\n",Type,reward/M);
	}
}
