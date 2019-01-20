#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<algorithm>
#include<cmath>
#include<ctime>
#include<windows.h>
using namespace std;
const int dir_index[4][2]={{0,1},{1,0},{0,-1},{-1,0}};
const double max_X=20;//the size of map
const double max_Y=20;//the size of map
const int discre_n=21;//discrete the map
const int subgoal_n=3;//max distance between current position and subgoal
const int fea_X=21;//the size of feature
const int fea_Y=21;//the size of feature
const int num_feature=fea_X+fea_Y;//the size of feature
const int deg_subgoal=3;
const int total_para=num_feature+2*deg_subgoal+1;
const int num_key=1;//the number of keys
const int num_kind=(1<<num_key);//"kind" denotes whether each key is gathered
const double err=0.2;//the error each step
const double dis_gather=1.0;//the max distance to gather items
const double eps=1e-6;
const double learning_rate=1e-3;
const double qlearning_rate=1e-1;
const int N = 100;
struct Point
{
	double x,y;
};
double Rand(double l,double r)
{
	unsigned int a=rand();
	a<<=10;
	a^=rand();
	a<<=10;
	a^=rand();
	double res=a/4294967295.0;
	res*=(r-l);
	res+=l;
	return res;
}
unsigned int intRand(unsigned int l,unsigned int r)
{
	unsigned int a=rand();
	a<<=10;
	a^=rand();
	a<<=10;
	a^=rand();
	unsigned int res=l+a%(r-l+1);
	return res;
}
double sgn(double x)
{
	if(x>0)
		return 1.0;
	else
		return -1.0;
}
double getLen(Point a)
{
	return sqrt(a.x*a.x+a.y*a.y);
}
double Dis(Point a,Point b)
{
	Point c;
	c.x=a.x-b.x;
	c.y=a.y-b.y;
	return getLen(c);
}
class Game
{
	/************
	A game has several elements,
	pos : the current position
	kind : current situation, whether keys are gotten
	score : total score the agent has recieved
	
	And some funcions,
	Init : initialize the game
	Action : execute an action
	getPos/getKind : get the current state
	getScore : get the current score
	************/
	private:
		Point pos;	//current position
		int kind;	//current situation
		double reward[num_kind];
		double score;
		Point dest;
		Point keys[num_key];
		void getNextPos(int dir)
		{
			//dir=0,1,2,3 means N,E,S,W
			double nx=dir_index[dir][0];
			double ny=dir_index[dir][1];
			pos.x+=nx*Rand(1-err,1+err);
			pos.y+=ny*Rand(1-err,1+err);
			pos.x=min(pos.x,max_X);
			pos.x=max(0.0,pos.x);
			pos.y=min(pos.y,max_Y);
			pos.y=max(0.0,pos.y);
		}
		void changeParas()
		{
			for(int i=0;i<num_key;i++)
			{
				if(Dis(keys[i],pos)<dis_gather)
					kind|=(1<<i);
			}
			if(Dis(dest,pos)<dis_gather)
			{
				score+=reward[kind];
			}
		}
		bool Ended()
		{
			return(Dis(dest,pos)<dis_gather);
		}
	
	public:
		void Init()
		{
			pos.x=2;
			pos.y=2;
			kind=0;
			score=0;
			for(int i=0;i<num_kind;i++)
			{
				if(i==0)
					reward[i]=1;
				else
					reward[i]=100;
			}
			dest.x=0;
			dest.y=0;
			keys[0].x=5;
			keys[0].y=5;
		}
		bool Action(int dir)
		{
			//return whether the game is ended
			getNextPos(dir);
			changeParas();
			return Ended();
		}
		Point getPos()
		{
			return pos;
		}
		int getKind()
		{
			return kind;
		}
		double getScore()
		{
			return score;
		}
};
class Agent
{
	/*************
	subgoal is dependent of pos,kind
	virtual_reward is dependent of pos,subgoal
	*************/
	private:
		double random_rate;
		Point subgoal;
		double w[4][total_para];
		double feature[total_para];
		Point pos;
		int kind;
		double score;
		bool need_new_subgoal;
		double value[4];
		int act;
		void getInfo(Game &g)
		{
			pos=g.getPos();
			kind=g.getKind();
			score=g.getScore();
		}
		double qvalue[discre_n][discre_n][2*subgoal_n+1][2*subgoal_n+1];
		int qi[N],qj[N],qk[N],ql[N],cnt;
		//qvalue[x1][y1][x2][y2]: now it is in (x1,y1), subgoal should set in (x2,y2)
		void makeNewSubgoal(Game &g)
		{
			Point np;
			int i=floor(pos.x/(max_X/(discre_n-1)));
			i=max(i,0);
			i=min(i,discre_n-1);
			int j=floor(pos.y/(max_Y/(discre_n-1)));
			j=max(j,0);
			j=min(j,discre_n-1);
			int vk=subgoal_n,vl=subgoal_n;
			
			for(int k=0;k<2*subgoal_n+1;k++)
			{
				for(int l=0;l<2*subgoal_n+1;l++)
				{
					if(qvalue[i][j][k][l]>qvalue[i][j][vk][vl])
					{
						if(0<=i+k-subgoal_n && i+k-subgoal_n<discre_n)
						{
							if(0<=j+l-subgoal_n && j+l-subgoal_n<discre_n)
							{
								//if(abs(k-subgoal_n)+abs(l-subgoal_n)>subgoal_n)
								{
									vk=k;
									vl=l;
								}
							}
						}
					}
				}
			}
			if(Rand(0.0,1.0)<random_rate)
			{
				while(1)
				{
					vk=intRand(0,2*subgoal_n);
					vl=intRand(0,2*subgoal_n);
					if(0<=i+vk-subgoal_n && i+vk-subgoal_n<discre_n)
					{
						if(0<=j+vl-subgoal_n && j+vl-subgoal_n<discre_n)
							break;
					}
				}
			}
			cnt++;
			qi[cnt]=i;
			qj[cnt]=j;
			qk[cnt]=vk;
			ql[cnt]=vl;
			vk+=i-subgoal_n;
			vl+=j-subgoal_n;
			np.x=vk*(max_X/(discre_n-1));
			np.y=vl*(max_Y/(discre_n-1));
			subgoal=np;
		}
		void getFeature(Game &g)
		{
			/*for(int i=0;i<fea_X;i++)
			{
				feature[i]=fabs(pos.x-i*max_X/(fea_X-1));
			}
			for(int i=0;i<fea_Y;i++)
			{
				feature[fea_X+i]=fabs(pos.y-i*max_Y/(fea_Y-1));
			}
			double v=1.0;
			for(int i=0;i<deg_subgoal;i++)
			{
				v*=2.0*subgoal.x/max_X;
				feature[num_feature+i]=v*3;
			}
			v=1.0;
			for(int i=0;i<=deg_subgoal;i++)
			{
				feature[num_feature+deg_subgoal+i]=v*3;
				v*=2.0*subgoal.y/max_Y;
			}*/
			memset(feature,0,sizeof feature);
			feature[0]=fabs(subgoal.x-pos.x);
			feature[1]=fabs(subgoal.y-pos.y);
			feature[2]=1.0;
			feature[3]=sgn(subgoal.x-pos.x);
			feature[4]=sgn(subgoal.y-pos.y);
			//feature[2]=feature[0]*feature[0];
			//feature[3]=feature[1]*feature[1];
		}
		double predictValue(int a)
		{
			double res=0;
			for(int i=0;i<total_para;i++)
			{
				res+=feature[i]*w[a][i];
			}
			return res;
		}
		double getVirtualReward(Game &g)
		{
			Point npos=g.getPos();
			double dis=fabs(npos.x-subgoal.x)+fabs(npos.y-subgoal.y);
			double res=-dis;
			return res;
		}
	
	public:
		void Reset()
		{
			memset(w,0,sizeof w);
			for(int i=0;i<discre_n;i++)
			{
				for(int j=0;j<discre_n;j++)
				{
					for(int k=0;k<2*subgoal_n+1;k++)
					{
						for(int l=0;l<2*subgoal_n+1;l++)
						{
							qvalue[i][j][k][l]=1e3;
						}
					}
				}
			}
		}
		void setRandomRate(double x)
		{
			random_rate=x;
		}
		void Init()
		{
			need_new_subgoal=1;
			subgoal.x=-1;
			subgoal.y=-1;
			cnt=0;
		}
		int makeAction(Game &g)
		{
			getInfo(g);
			if(need_new_subgoal)
			{
				makeNewSubgoal(g);
				need_new_subgoal=0;
			}
			getFeature(g);
			act=0;
			for(int i=0;i<=3;i++)
			{
				value[i]=predictValue(i);
				if(value[i]>value[act])
					act=i;
			}
			if(Rand(0.0,1.0)<random_rate)
			{
				act=intRand(0,3);
			}
			return act;
		}
		void Update(Game &g)
		{
			Point npos=g.getPos();
			if(Dis(subgoal,npos)<0.2*dis_gather)
				need_new_subgoal=1;
			for(int i=0;i<total_para;i++)
			{
				double prac_reward=getVirtualReward(g);
				w[act][i]+=learning_rate*feature[i]*(prac_reward-value[act]);
			}
		}
		void endOnce(double x)
		{
			for(int i=1;i<=cnt;i++)
			{
				qvalue[qi[i]][qj[i]][qk[i]][ql[i]]+=qlearning_rate*(x-qvalue[qi[i]][qj[i]][qk[i]][ql[i]]);
			}
		}
		void Print(Game &g)
		{
			Point npos=g.getPos();
			int nkind=g.getKind();
			double nscore=g.getScore();
			double virsc=getVirtualReward(g);
			printf("-----------");
			printf("pos: (%.3f , %.3f) --> (%.3f , %.3f)\n",pos.x,pos.y,npos.x,npos.y);
			printf("kind: %d --> %d\n",kind,nkind);
			printf("score: %.1f\n",nscore);
			printf("subgoal: (%.3f , %.3f)\n",subgoal.x,subgoal.y);
			printf("vir_score: %.1f\n",virsc);
			printf("value: %.1f %.1f %.1f %.1f\n",value[0],value[1],value[2],value[3]);
			printf("-----------\n\n");
		}
		void Printx()
		{
			//for(int i=0;i<discre_n;i++)
			{
				//for(int j=0;j<discre_n;j++)
				{
					int i=0,j=0;
					for(int k=0;k<2*subgoal_n+1;k++)
					{
						for(int l=0;l<2*subgoal_n+1;l++)
						{
							printf("%.1f ",qvalue[i][j][k][l]);
						}
						printf("\n");
					}
					printf("\n");
				}
				printf("\n");printf("\n");
			}
		}
};
const int Times = 1000;
double scs[Times];
int main()
{
	srand(time(0));
	Game g;
	Agent a;
	a.Reset();
	int cc=0;
	for(int i=0;i<Times;i++)
	{
		g.Init();
		//if(i<Times/2)
			a.setRandomRate(0.3*(Times-i)/Times);
			a.Init();
		//else
		//	a.setRandomRate(0.0);
		bool terminal=0;
		while(!terminal)
		{
			int y=a.makeAction(g);
			if(g.Action(y))
				terminal=1;
			a.Update(g);
			cc++;
			/*if(i==Times-1)
			{
				{
					printf("TIME = %d-------------------------\n\n\n",i);
					a.Print(g);
					printf("\n\n\n\n\n\n");
					//Sleep(1000);
				}
				cc=0;
			}*/
		}
		scs[i]=g.getScore();
		a.endOnce(scs[i]);
	}
	for(int i=1;i<Times;i++)
		scs[i]+=scs[i-1];
	double last=0;
	for(int i=(Times/100)-1;i<Times;i+=(Times/100))	
	{
		printf("%.1f,",(scs[i]-last)/(Times/100));
		last=scs[i];
	}
	//a.Printx();
	return 0;
}
