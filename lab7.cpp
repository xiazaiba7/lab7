#include <iostream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sstream>
#include <queue>
#include <stack>
#include <iostream>
using namespace std;
int length=0; 
int num;
int numb=-1;
int blocknum;
int tempv;
string basepoint;//用来存储一维数组的首地址 
bool constdef=false;
char line[1050];
string varname;
string constname;
FILE *in,*out;
string letter[100000];
char op[20000];
stack<int> judge_blocks;
stack<int> out_blocks; 
int shuzublock; 
bool isshuzudef; 
bool ismain=false;
struct ident 
{
	string name;
	string name2;
	int value;
	int type; //1表示变量，0表示常量, 2表示变量的值（1是指针） 
};
struct shuzu
{
	string name;
	string name2;
	int length;
	vector<int> value;
	int type; //1表示一维数组，2表示二维数组 
	int yiweilength;
	int erweilength;
	int isconst;
};
struct identtable
{
	ident idents[300];
	vector<shuzu> shuzus;
	int top;
	int outnum;//上层符号表的编号 
};
identtable identstable[1000];
ident shuzi[200000];
int top1=-1,top2=-1;
string temp;
int tempvalue;

int operatewithnoprint(char c)
{	
	if(c=='+')
	{
		shuzi[top1-1].value=shuzi[top1].value+shuzi[top1-1].value;
	}
	if(c=='-')
	{	
		shuzi[top1-1].value=shuzi[top1-1].value-shuzi[top1].value;
	}
	if(c=='*')
	{
		shuzi[top1-1].value=shuzi[top1-1].value*shuzi[top1].value;
	}
	if(c=='/')
	{
		shuzi[top1-1].value=shuzi[top1-1].value/shuzi[top1].value;
	}
	if(c=='%')
	{
		shuzi[top1-1].value=shuzi[top1-1].value%shuzi[top1].value;
	}
	top1--;
	return shuzi[top1-1].value; 
}
int operate(char c)
{
	string num1="",num2="";
	char ch[10];
	if(shuzi[top1].type==1)//是变量 
	{
		fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,shuzi[top1].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[top1].name2=ch;
		shuzi[top1].type=2; 
	}
	if(shuzi[top1-1].type==1)//是变量
	{
		fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,shuzi[top1-1].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[top1-1].name2=ch;
		shuzi[top1-1].type=2;
	} 
	if(shuzi[top1].type==2)
	{
		num1=shuzi[top1].name2;
	}
	else
	{
		sprintf(ch,"%d",shuzi[top1].value);
		num1=ch;
	}
	if(shuzi[top1-1].type==2)
	{
		num2=shuzi[top1-1].name2;
	}
	else
	{
		sprintf(ch,"%d",shuzi[top1-1].value);
		num2=ch;
	}
	
	if(c=='+')
	{
		fprintf(out,"          %%x%d = add i32 %s,%s\n",++numb,num2.c_str(),num1.c_str());
		shuzi[top1-1].value=shuzi[top1].value+shuzi[top1-1].value;
		sprintf(ch,"%%x%d",numb);
		shuzi[top1-1].name2=ch;
		shuzi[top1-1].type=2;
		shuzi[top1-1].name="";//叫啥都无所谓了 
	}
	if(c=='-')
	{	
		fprintf(out,"          %%x%d = sub i32 %s,%s\n",++numb,num2.c_str(),num1.c_str());
		shuzi[top1-1].value=shuzi[top1-1].value-shuzi[top1].value;
		sprintf(ch,"%%x%d",numb);
		shuzi[top1-1].name2=ch;
		shuzi[top1-1].type=2;
		shuzi[top1-1].name="";//叫啥都无所谓了 
	}
	if(c=='*')
	{
		fprintf(out,"          %%x%d = mul i32 %s,%s\n",++numb,num2.c_str(),num1.c_str());
		shuzi[top1-1].value=shuzi[top1-1].value*shuzi[top1].value;
		sprintf(ch,"%%x%d",numb);
		shuzi[top1-1].name2=ch;
		shuzi[top1-1].type=2;
		shuzi[top1-1].name="";//叫啥都无所谓了 
	}
	if(c=='/')
	{
		fprintf(out,"          %%x%d = sdiv i32 %s,%s\n",++numb,num2.c_str(),num1.c_str());
		if(shuzi[top1].value!=0)
		{
			shuzi[top1-1].value=shuzi[top1-1].value/shuzi[top1].value;
		}
		sprintf(ch,"%%x%d",numb);
		shuzi[top1-1].name2=ch;
		shuzi[top1-1].type=2;
		shuzi[top1-1].name="";//叫啥都无所谓了 
	}
	if(c=='%')
	{
		fprintf(out,"          %%x%d = srem i32 %s,%s\n",++numb,num2.c_str(),num1.c_str());
		if(shuzi[top1].value!=0)
		{
			shuzi[top1-1].value=shuzi[top1-1].value%shuzi[top1].value;
		}
		sprintf(ch,"%%x%d",numb);
		shuzi[top1-1].name2=ch;
		shuzi[top1-1].type=2;
		shuzi[top1-1].name="";//叫啥都无所谓了 
	}
	top1--;
	return shuzi[top1].value; 
}
void PrintRel(int judge,ident yuan)
{
	char ch[10];
	if(yuan.type==1)
	{
		fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,yuan.name2.c_str());
		sprintf(ch,"%%x%d",numb);
		yuan.name2=ch;
	}
	if(yuan.type==0)
	{
		sprintf(ch,"%d",yuan.value);
		yuan.name2=ch;
	}
	if(shuzi[0].type==1)
	{
		fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
	}
	if(shuzi[0].type==0)
	{
		sprintf(ch,"%d",shuzi[0].value);
		shuzi[0].name2=ch;
	}
	if(judge==1)
	{
		fprintf(out,"          %%x%d = icmp sgt i32 %s, %s\n",++numb,yuan.name2.c_str(),shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
		shuzi[0].type=3;
	}
	else if(judge==2)
	{
		fprintf(out,"          %%x%d = icmp slt i32 %s, %s\n",++numb,yuan.name2.c_str(),shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
		shuzi[0].type=3;
	}
	else if(judge==3)
	{
		fprintf(out,"          %%x%d = icmp sge i32 %s, %s\n",++numb,yuan.name2.c_str(),shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
		shuzi[0].type=3;
	}
	else if(judge==4)
	{
		fprintf(out,"          %%x%d = icmp sle i32 %s, %s\n",++numb,yuan.name2.c_str(),shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
		shuzi[0].type=3;
	}
}
void PrintEq(int judge,ident yuan)
{
	char ch[10];
	if(yuan.type==1)
	{
		fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,yuan.name2.c_str());
		sprintf(ch,"%%x%d",numb);
		yuan.name2=ch;
	}
	if(yuan.type==0)
	{
		sprintf(ch,"%d",yuan.value);
		yuan.name2=ch;
	}
	if(shuzi[0].type==1)
	{
		fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
	}
	if(shuzi[0].type==0)
	{
		sprintf(ch,"%d",shuzi[0].value);
		shuzi[0].name2=ch;
	}
	if(judge==1)
	{
		fprintf(out,"          %%x%d = icmp eq i32 %s, %s\n",++numb,yuan.name2.c_str(),shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
		shuzi[0].type=3;
	}
	else if(judge==2)
	{
		fprintf(out,"          %%x%d = icmp ne i32 %s, %s\n",++numb,yuan.name2.c_str(),shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
		shuzi[0].type=3;
	}
 } 
void PrintLAnd(ident yuan)
{
	char ch[10];
	if(yuan.type==3)
	{
		fprintf(out,"          %%x%d = zext i1 %s to i32\n",++numb,yuan.name2.c_str());
		sprintf(ch,"%%x%d",numb);
		yuan.name2=ch;
	}
	if(yuan.type==1)
	{
		fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,yuan.name2.c_str());
		sprintf(ch,"%%x%d",numb);
		yuan.name2=ch;
	}
	if(yuan.type==0)
	{
		sprintf(ch,"%d",yuan.value);
		yuan.name2=ch;
	}
	if(shuzi[0].type==3)
	{
		fprintf(out,"          %%x%d = zext i1 %s to i32\n",++numb,shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
	}
	if(shuzi[0].type==1)
	{
		fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
	}
	if(shuzi[0].type==0)
	{
		sprintf(ch,"%d",shuzi[0].value);
		shuzi[0].name2=ch;
	}
	fprintf(out,"          %%x%d = and i32 %s, %s\n",++numb,yuan.name2.c_str(),shuzi[0].name2.c_str());
	sprintf(ch,"%%x%d",numb);
	shuzi[0].name2=ch;
	shuzi[0].type=2;
}
void PrintLOr(ident yuan)
{
	char ch[10];
	if(yuan.type==3)
	{
		fprintf(out,"          %%x%d = zext i1 %s to i32\n",++numb,yuan.name2.c_str());
		sprintf(ch,"%%x%d",numb);
		yuan.name2=ch;
	}
	if(yuan.type==1)
	{
		fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,yuan.name2.c_str());
		sprintf(ch,"%%x%d",numb);
		yuan.name2=ch;
	}
	if(yuan.type==0)
	{
		sprintf(ch,"%d",yuan.value);
		yuan.name2=ch;
	}
	if(shuzi[0].type==3)
	{
		fprintf(out,"          %%x%d = zext i1 %s to i32\n",++numb,shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
	}
	if(shuzi[0].type==1)
	{
		fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,shuzi[0].name2.c_str());
		sprintf(ch,"%%x%d",numb);
		shuzi[0].name2=ch;
	}
	if(shuzi[0].type==0)
	{
		sprintf(ch,"%d",shuzi[0].value);
		shuzi[0].name2=ch;
	}
	fprintf(out,"          %%x%d = or i32 %s, %s\n",++numb,yuan.name2.c_str(),shuzi[0].name2.c_str());
	sprintf(ch,"%%x%d",numb);
	shuzi[0].name2=ch;
	shuzi[0].type=2;
}
int q[100];
int top=0;
int result= -100862;
int MulExp(int index);
int PrimaryExp(int index);
int AddExp(int index);
int UnaryExp(int index);
int ConstDef(int index);
int inputstack[100];
int opstack[100];
int ConstDecl(int index);
int VarDecl(int index);
int Exp(int index);
int Vardef(int index);
int VarDecl(int index);
int Stmt(int index); 
int Decl(int index);
int Blockitem(int index);
int FuncRParams(int index);
int RelExp(int index);
int EqExp(int index);
int LAndExp(int index);
int LOrExp(int index);
int Cond(int index);
int ConstInitVal(int index);
int InitVal(int index);
int computeshuzi(int index);
int Lval();
int symbol(string s)
{
	if(s=="(")
	{
		fprintf(out,"(");
		num++;
		return 3;
	}
	else if(s==")")
	{	
		fprintf(out,")");
		num++;
		return 4;
	}	
	else if(s=="{")
	{
		fprintf(out,"{\n");
		num++;
		return 5;
	}
	else if(s=="}")
	{
		fprintf(out,"}");
		num++;
		return 9;
	}
	else if(s==";")
	{
		num++;
		return 8;
	}
	else
		return 0;
} 
 int judgeword(string s,int n)
 {
 	int j=n+1;
 	while(1)
	{
		if((letter[j]>="a"&&letter[j]<="z"||letter[j]>="0"&&letter[j]<="9"||letter[j]>="A"&&letter[j]<="Z"||letter[j]=="_")&&letter[j]!="block")
		{
   			s=(s+letter[j]).c_str();
   			j++;
   		}
   		else
   		{
   			break;
		}
  	}
  	num=j;
 	if(strcmp(s.c_str(),"int")==0)
 	{
 		return 1;
 	}
 	else if(strcmp(s.c_str(),"main")==0)
 	{
 		ismain=true;
 		return 2;
 	}
 	else if(strcmp(s.c_str(),"return")==0)
 	{
	 	
 		return 6;
 	}
 	else if(strcmp(s.c_str(),"const")==0)
 		return 7;
 	else if(strcmp(s.c_str(),"getint")==0)
 		return 10;
 	else if(strcmp(s.c_str(),"getch")==0)
 		return 11;
 	else if(strcmp(s.c_str(),"getarray")==0)
 		return 12;
 	else if(strcmp(s.c_str(),"putint")==0)
 		return 13;
 	else if(strcmp(s.c_str(),"putch")==0)
 		return 14;
 	else if(strcmp(s.c_str(),"putarray")==0)
 		return 15;
 	else if(strcmp(s.c_str(),"if")==0)
 	{
 		return 20;
	}
	else if(strcmp(s.c_str(),"else")==0)
 	{
 		return 21;
	}
	else if(strcmp(s.c_str(),"while")==0)
	{
		return 22;
	}
	else if(strcmp(s.c_str(),"continue")==0)
	{
		return 23;
	}
	else if(strcmp(s.c_str(),"break")==0)
	{
		return 24;
	}
 	else
 	{
 		temp=s;
 		return 3;
 	}
 }
 bool isnonezero(string s)
 {
 	if(s>="1"&&s<="9")
 		return true;
	return false; 
 }
 int computeshuzi(int index)
{
	if(index>0)
	{
		while(top2!=-1)
		{
			operate(op[top2]);
			top2--;
		}
	}
	else
	{
		while(top2!=-1)
		{
			operatewithnoprint(op[top2]);
			top2--;
		}
	}
	return shuzi[0].value;
}
 bool isoctaldigit(string s)
 {
 	if(s>="0"&&s<="7")
 		return true;
 	else
 		return false;
 }
 bool ishexdigit(string s)
 {
 	if((s>="0"&&s<="9"||s>="a"&&s<="f"||s>="A"&&s<="F")&&s!="block")
 		return true;
 	else
 		return false;
 }
int Blockitem(int index)//index表示符号表的编号 
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j = num;
	if(Decl(index)>0)
	{
		return 1;	
	}	
	else
	{
		num = j;
		if(Stmt(index)>0)
			return 2;
		else
		{
			printf("%s",temp.c_str());
			printf("1\n");
			return 0;
		}
	}
}
int Decl(int index)
{
	if(letter[num]=="c")
	{
		if(ConstDecl(index)>0)
			return 1;
		else
		{	
			printf("%s",temp.c_str());
			printf("2\n"); 
			return 0;
		}
	}
	else if(letter[num]=="i")
	{
		if(VarDecl(index)>0)
			return 2;
		else
		{
			printf("%s",temp.c_str());
			printf("3\n");
			return 0;
		}
	}
	else
		return 0;
}
int ConstDecl(int index)
{
	if((letter[num]>="a"&&letter[num]<="z")||(letter[num]>="A"&&letter[num]<="Z")||letter[num]=="_")
	{
		int a = judgeword(letter[num],num);
		if(a==7)
		{
			while(letter[num]=="block")
			{
				num++;
			}
			int b = judgeword(letter[num],num);
			while(letter[num]=="block")
			{
				num++;
			}
			if(b==1)
			{			
				if(ConstDef(index)>0)
				{
					while(letter[num]==",")
					{
						num++;
						if(ConstDef(index)<=0)
						{
							return 0;
						}
						while(letter[num]=="block")
						{
							num++;
						} 
					}
					while(letter[num]=="block")
					{
						num++;
					}
					if(letter[num]==";")
					{
						num++;
						return 1;
					}
					else
					{
						return 0;
					}
					
				}
				else
				{
					return 0;
				}
				
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	return 0;
}
int ConstDef(int index)
{
	top1=-1;
	top2=-1;
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if((letter[num]>="a"&&letter[num]<="z")||(letter[num]>="A"&&letter[num]<="Z")||letter[num]=="_")
	{
		int a = judgeword(letter[num],num);
		for(int i=0;i<=identstable[index].top;i++)
		{
			if(identstable[index].idents[i].name==temp)
			{
				return 0;
			}
		}
		while(letter[num]=="block")
		{
			num++;
		}
		if(a==3)
		{
			int type=1;
			constname=temp;

			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]=="[")
			{
				isshuzudef=true;
				int weidu=0;
				shuzu newshuzu;
				newshuzu.name=constname;
				newshuzu.length=0;
				for(int i=0;i<identstable[index].shuzus.size();i++)
				{
					if(identstable[index].shuzus[i].name==constname)
					{
						return 0;
					}
				}
				if(index>0)
				{
					numb++;
					char ch[50];
					sprintf(ch,"%%x%d",numb);
					newshuzu.name2=ch;
				}
				else
				{
					char ch[50];
					sprintf(ch,"@%s",newshuzu.name.c_str());
					newshuzu.name2=ch;
				}
				newshuzu.length=1;
				newshuzu.isconst=1;
				identstable[index].shuzus.push_back(newshuzu);
				while(letter[num]=="[")
				{
					constdef=true;
					type++;
					top1=-1;
					top2=-1;
					weidu++;
					num++;
					while(letter[num]=="block")
					{
						num++;
					}
					constdef==true;
					if(Exp(index)>0)
					{
						computeshuzi(index);
						constdef=false;
						while(letter[num]=="block")
						{
							num++;
						}
						if(letter[num]=="]")
						{
							if(weidu==1)
							{
								identstable[index].shuzus.back().yiweilength=shuzi[0].value;
							}
							else if(weidu==2)
							{
								identstable[index].shuzus.back().erweilength=shuzi[0].value;
							}
							identstable[index].shuzus.back().type=type-1;
							identstable[index].shuzus.back().length*=shuzi[0].value;
							num++;
						}
						else
						{
							return 0;
						}
					}
					else
					{
						return 0;
					}
					while(letter[num]=="block")
					{
						num++;
					}	
				}
				if(index>0)
				{
					fprintf(out,"          %s = alloca [%d x i32]\n",identstable[index].shuzus.back().name2.c_str(),identstable[index].shuzus.back().length);
					basepoint=identstable[index].shuzus.back().name2;
				}
			}
			else
			{
				for(int i=0;i<=identstable[index].top;i++)
				{
					if(identstable[index].idents[i].name==constname)
						return 0;
				}
				identstable[index].idents[++identstable[index].top].name=constname;
				identstable[index].idents[identstable[index].top].type=0;
					
				identstable[index].idents[identstable[index].top].name2="";
			}
			if(letter[num]=="=")
			{
				num++;
				while(letter[num]=="block")
				{
					num++;
				}
				if(type==1)
				{
					constdef=true;
					if(ConstInitVal(index)>0)
					{
						computeshuzi(index);
						identstable[index].idents[identstable[index].top].value=shuzi[0].value;
						constdef=false;
						return type;
					}
					else
					{
						return 0;
					}
				}
				else if(type>=2)
				{
					if(index>=1)
					{
						numb++;
						char ch[50];
						sprintf(ch,"%%x%d",numb);
						basepoint = ch;
						int length=identstable[index].shuzus.back().length;
						fprintf(out,"          %s = getelementptr [%d x i32],[%d x i32]* %s, i32 0, i32 0\n",basepoint.c_str(),length,length,identstable[index].shuzus.back().name2.c_str());
					}
					if(ConstInitVal(index)>0)
					{
						return type;
					}
				}
			}
			else
			{
				
				return 0;
			}
			
		}
		else
		{
			num=j;
			return 0;
		}
	}
	else
	{
		num=j;
		return 0;
	}
}
int ConstInitVal(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int a=num;
	if(letter[num]=="{")
	{
		constdef=true;
		top1=-1;
		top2=-1;
		int address=0;
		shuzublock=0;
		num++;
		while(letter[num]=="block")
		{
			num++;
		}
		if(letter[num]=="}")
		{
			while(address+1<identstable[index].shuzus.back().length)
			{	
				address++;
				identstable[index].shuzus.back().value.push_back(0);	
			}
			constdef=false;
			num++;
			return 2;
		}
		int yiwei=0;
		int numcan=0; 
		if(letter[num]=="{")
		{
			//二维数组 
			if(identstable[index].shuzus.back().type!=2)
			{
				return 0;
			}
			while(letter[num]=="{")
			{
				top1=-1;
				top2=-1;
				yiwei++;
				num++;
				while(letter[num]=="block")
				{
					num++;
				}
				
				while(Exp(index)>0)
				{
					computeshuzi(index);
					identstable[index].shuzus.back().value.push_back(shuzi[0].value);
					if(index>0)
					{
						fprintf(out,"          %%x%d = getelementptr i32,i32* %s, i32 %d\n",++numb,basepoint.c_str(),address);
						fprintf(out,"          store i32 %d, i32* %%x%d\n",shuzi[0].value,numb);
					}
					while(letter[num]=="block")
					{
						num++;
					}
					if(letter[num]==",")
					{
						top1=-1;
						top2=-1;
						address++;
						num++;
					}
					else
					{
						break;
					}
					while(letter[num]=="block")
					{
						num++;
					}
				}
				while(letter[num]=="block")
				{
					num++;
				}
				if(letter[num]=="}")
				{
					num++;	
				}	
				while(letter[num]=="block")
				{
					num++;
				}
				if(letter[num]==",")
				{
					num++;
				}
				while(letter[num]=="block")
				{
					num++;
				}
				if(address>=yiwei*identstable[index].shuzus.back().erweilength)
				{
					return 0;
				}
				while(address+1<identstable[index].shuzus.back().erweilength)
				{	
					address++;
					identstable[index].shuzus.back().value.push_back(0);	
				}	
				address++;
			} 
		}
		else
		{
			top1=-1;
			top2=-1;
			while(letter[num]=="block")
			{
					num++;
			}
			while(Exp(index)>0)
			{
				computeshuzi(index);
				identstable[index].shuzus.back().value.push_back(shuzi[0].value);
				if(index>0)
				{
					fprintf(out,"          %%x%d = getelementptr i32,i32* %s, i32 %d\n",++numb,basepoint.c_str(),address);
					fprintf(out,"          store i32 %d, i32* %%x%d\n",shuzi[0].value,numb);
				}
				while(letter[num]=="block")
				{
					num++;
				}
				if(letter[num]==",")
				{
					top1=-1;
					top2=-1;
					address++;
					num++;
				}
				else
				{
					break;
				}
				while(letter[num]=="block")
				{
					num++;
				}
			}
			if(address>=identstable[index].shuzus.back().length)
			{
				return 0;
			}
			while(address+1<identstable[index].shuzus.back().length)
			{	
				address++;
				identstable[index].shuzus.back().value.push_back(0);	
			}
			address++;
		}
		
		while(letter[num]=="block")
		{	
			num++;
		}
		if(letter[num]=="}")
		{
			while(address+1<identstable[index].shuzus.back().length)
			{	
				address++;
				identstable[index].shuzus.back().value.push_back(0);	
			}
			constdef=false;
			num++;
			return 2;
		}	
	}
	num=a;
	if(Exp(index)>0)
	{
		return 1;
	}
}
int VarDecl(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if((letter[num]>="a"&&letter[num]<="z")||(letter[num]>="A"&&letter[num]<="Z")||letter[num]=="_")
	{
		int a = judgeword(letter[num],num);
		if(a==1)
		{
			numb++;
			int address=numb;
			int b = Vardef(index);
			if(b>0)
			{	
				while(letter[num]=="block")
				{
					num++;
				}
				while(letter[num]==",")
				{
					num++;
					int c = Vardef(index);
					if(c<=0)
					{
						return 0;
					}
				}
				while(letter[num]=="block")
				{
					num++;
				}
				if(letter[num]==";")
				{
					num++;
					return 1;
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			num=j;
			return 0;
		}
	}
	else
	{
		num=j;
		return 0;
	}
}
int Lval()
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	int a=judgeword(letter[num],num);
	int numkuohao=0;
	if(a==3)
	{
		while(letter[num]=="block")
		{
			num++;
		}
		while(letter[num]=="[")
		{	
			if(letter[num]!="[")
			{
				numkuohao++;
			}
			if(letter[num]=="]")
			{
				numkuohao--;
			}
			num++;
			while(letter[num]=="block")
			{
				num++;
			}
			if(numkuohao==0)
			{
				continue;
			}
		}
		while(letter[num]=="block")
		{
			num++;
		}
		if(letter[num]=="=")
		{
			num=j;
			return 1;
		}
		else
		{
			num=j;
			return 2;
		}
	}
	num=j;
	return 2;
	
 } 
int InitVal(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int a=num;
	if(letter[num]=="{")
	{
		top1=-1;
		top2=-1;
		int address=0;
		shuzublock=0;
		num++;
		while(letter[num]=="block")
		{
			num++;
		}
		if(letter[num]=="}")
		{
			while(address+1<identstable[index].shuzus.back().length)
			{	
				address++;
				identstable[index].shuzus.back().value.push_back(0);	
			}
			constdef=false;
			num++;
			return 2;
		}
		int yiwei=0;
		if(letter[num]=="{")
		{
			//二维数组 
			if(identstable[index].shuzus.back().type!=2)
			{
				return 0;
			}
			while(letter[num]=="{")
			{
				top1=-1;
				top2=-1;
				yiwei++;
				num++;
				while(letter[num]=="block")
				{
					num++;
				}
				while(Exp(index)>0)
				{
					computeshuzi(index);
					identstable[index].shuzus.back().value.push_back(shuzi[0].value);
					if(index>0)
					{
						fprintf(out,"          %%x%d = getelementptr i32,i32* %s, i32 %d\n",++numb,basepoint.c_str(),address);
						fprintf(out,"          store i32 %d, i32* %%x%d\n",shuzi[0].value,numb);
					}
					//赋值 
					while(letter[num]=="block")
					{
						num++;
					}
					if(letter[num]==",")
					{
						top1=-1;
						top2=-1;
						address++;
						num++;
					}
					else
					{
						break;
					}
					while(letter[num]=="block")
					{
						num++;
					}
				}
				while(letter[num]=="block")
				{
					num++;
				}
				if(letter[num]=="}")
				{
					num++;	
				}	
				while(letter[num]=="block")
				{
					num++;
				}
				if(letter[num]==",")
				{
					num++;
				}
				while(letter[num]=="block")
				{
					num++;
				}
				if(address>=yiwei*identstable[index].shuzus.back().erweilength)
				{
					return 0;
				}
				while(address+1<identstable[index].shuzus.back().erweilength)
				{	
					address++;
					identstable[index].shuzus.back().value.push_back(0);	
				}	
				address++;
			} 
		}
		else
		{
			top1=-1;
			top2=-1;
			if(identstable[index].shuzus.back().type!=1)
			{
				return 0;
			}
			while(letter[num]=="block")
			{
					num++;
			}
			while(Exp(index)>0)
			{
				computeshuzi(index);
				identstable[index].shuzus.back().value.push_back(shuzi[0].value);
				if(index>0)
				{
					fprintf(out,"          %%x%d = getelementptr i32,i32* %s, i32 %d\n",++numb,basepoint.c_str(),address);
					fprintf(out,"          store i32 %d, i32* %%x%d\n",shuzi[0].value,numb);
				}
				//赋值 
				while(letter[num]=="block")
				{
					num++;
				}
				if(letter[num]==",")
				{
					top1=-1;
					top2=-1;
					address++;
					num++;
				}
				else
				{
					break;
				}
				while(letter[num]=="block")
				{
					num++;
				}
			}
			if(address>=identstable[index].shuzus.back().length)
			{
				return 0;
			}
			while(address+1<identstable[index].shuzus.back().length)
			{	
				address++;
				identstable[index].shuzus.back().value.push_back(0);	
			}
			address++;
		}
		
		while(letter[num]=="block")
		{	
			num++;
		}
		if(letter[num]=="}")
		{
			while(address+1<identstable[index].shuzus.back().length)
			{	
				address++;
				identstable[index].shuzus.back().value.push_back(0);	
			}
			num++;
			return 2;
		}	
	}
	num=a;
	if(Exp(index)>0)
	{
		return 1;
	}
	return 0;
}
int Vardef(int index)
{
	top1=-1;
	top2=-1;
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if((letter[num]>="a"&&letter[num]<="z")||(letter[num]>="A"&&letter[num]<="Z")||letter[num]=="_")
	{
		int a = judgeword(letter[num],num);
		if(a==3)
		{
			int type=1;
			int length=1;
			varname=temp;
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]=="[")
			{
				
				int weidu=0;
				shuzu newshuzu;
				newshuzu.name=varname;
				newshuzu.length=0;
				newshuzu.isconst=0;
				if(index>0)
				{
					numb++;
					char ch[50];
					sprintf(ch,"%%x%d",numb);
					newshuzu.name2=ch;
				}
				else
				{
					char ch[50];
					sprintf(ch,"@%s",varname.c_str());
					newshuzu.name2=ch;
				}
				newshuzu.length=1;
				identstable[index].shuzus.push_back(newshuzu);
				while(letter[num]=="[")
				{
					top1=-1;
					top2=-1;
					weidu++;
					num++;
					type++;
					while(letter[num]=="block")
					{
						num++;
					}
					constdef==true;
					if(Exp(index)>0)
					{
						computeshuzi(index);
						constdef=false;
						while(letter[num]=="block")
						{
							num++;
						}
						if(letter[num]=="]")
						{
							if(weidu==1)
							{
								identstable[index].shuzus.back().yiweilength=shuzi[0].value;
							}
							else if(weidu==2)
							{
								identstable[index].shuzus.back().erweilength=shuzi[0].value;
							}
							identstable[index].shuzus.back().type=type-1;
							identstable[index].shuzus.back().length*=shuzi[0].value;
							num++;
						}
						else
						{
							return 0;
						}
					}
					else
					{
						return 0;
					}
					while(letter[num]=="block")
					{
						num++;
					}	
				}
				if(index>0)
				{
					fprintf(out,"          %s = alloca [%d x i32]\n",identstable[index].shuzus.back().name2.c_str(),identstable[index].shuzus.back().length);
					basepoint=identstable[index].shuzus.back().name2;
				}
			}
			else
			{
				type=1;
				numb++;
				string name2;
				char ch[50];
				sprintf(ch,"%%x%d",numb);
				name2=ch;
				int address=numb;
				fprintf(out,"          %%x%d = alloca i32\n",address);
				identstable[index].idents[++identstable[index].top].type=1;
				identstable[index].idents[identstable[index].top].name2=name2;
				for(int i=1;i<=identstable[index].top;i++)//防止重复定义某一变量 
				{
					if(identstable[index].idents[i].name==varname)
						return 0;
				}
				identstable[index].idents[identstable[index].top].name=varname;
				identstable[index].idents[identstable[index].top].type=1;
				identstable[index].idents[identstable[index].top].value=0;
			}
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]=="=")
			{
				num++;
				while(letter[num]=="block")
				{
					num++;
				}
				if(type==1)
				{
					if(InitVal(index)>0)
					{
//						while(top2!=-1)
//						{
//							operate(op[top2]);
//							top2--;
//						}
						computeshuzi(index);
						if(shuzi[0].type==0)
						{
							fprintf(out,"          store i32 %d, i32* %s\n",shuzi[0].value,identstable[index].idents[identstable[index].top].name2.c_str());
						}
						else if(shuzi[0].type==2)
						{
							fprintf(out,"          store i32 %s, i32* %s\n",shuzi[0].name2.c_str(),identstable[index].idents[identstable[index].top].name2.c_str());
						}
						else if(shuzi[0].type==1)
						{
							fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,shuzi[0].name2.c_str());
							fprintf(out,"          store i32 %%x%d, i32* %s\n",numb,identstable[index].idents[identstable[index].top].name2.c_str());
						}
						identstable[index].idents[identstable[index].top].value=shuzi[0].value;//这里可能要修改 
						return 2;
						
					}
					else
					{
						return 0;
					}
				}
				else if(type>=2)
				{	
					if(index>=1)
					{
						numb++;
						char ch[50];
						sprintf(ch,"%%x%d",numb);
						basepoint = ch;
						int length=identstable[index].shuzus.back().length;
						fprintf(out,"           %s = getelementptr [%d x i32],[%d x i32]* %s, i32 0, i32 0\n",basepoint.c_str(),length,length,identstable[index].shuzus.back().name2.c_str());
					}
					if(InitVal(index)>0)
					{
						return 2;
					}
				}
			}
			else
			{
				if(type>=2)
				{
					for(int i=0;i<identstable[index].shuzus.back().length;i++)
					{
						identstable[index].shuzus.back().value.push_back(0);
					} 
				}
				return 1;
			}
		}
		else
		{
			num=j;
			return 0;
		}
	}
	else
	{
		num=j;
		return 0;
	}
	return 1;
}
int Stmt(int index)
{
	char ch[10];
	top1=-1;
	top2=-1;
	while(letter[num]=="block")
	{
		num++;
	}
	int j = num ;
	if(letter[num]==";")
	{
		num++;
		return 3;
	}
	if(Lval()==2)
	{
		if(Exp(index)>0)
		{
			int biaoji=0;
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]=="=")
			{
				goto part2;
			}
			while(top2!=-1)
			{
				operate(op[top2]);
				top2--;
			}
			if(letter[num]==";")
			{
				num++;
				return 3;
			}
			else
			{
				printf("4\n");
				return 0;
			}
		}
	}
	part2:
	num=j;
	top1=-1;
	top2=-1;
	if(letter[num]=="{")//新建一个block 
	{
		num++;
		blocknum++;
		int newindex=blocknum;
		while(letter[num]=="block")
		{
			num++;
		}
		identstable[newindex].outnum=index;
		identstable[newindex].top=0;
		if(Blockitem(newindex)>0)
		{
			while(letter[num]=="block")
			{
				num++;
			}
			int x=num;
			while(Blockitem(newindex)>0)
			{
				while(letter[num]=="block")
				{
					num++;
				}
				x=num;
			}
			num=x;
			if(letter[num]=="}")
			{
				num++;
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}	
	}
	num=j;
	if((letter[num]>="a"&&letter[num]<="z")||(letter[num]>="A"&&letter[num]<="Z")||letter[num]=="_")
	{
		num=j;
		int a = judgeword(letter[num],num);
		ident yiwei;
		ident erwei;
		yiwei.type=10;
		erwei.type=10;
		ident address;
		address.value=-1;
		if(a==6)//return
		{
			if(Exp(index)>0)
			{
				while(letter[num]=="block")
				{
					num++;
				}
				while(top2!=-1)
				{
					operate(op[top2]);
					top2--;
				}
				if(letter[num]==";")
				{
					num++;
					
					if(shuzi[0].type==0)
					{
						fprintf(out,"          ret i32 ");
						fprintf(out,"%d\n",shuzi[0].value);
					}
					else if(shuzi[0].type==2)
					{
						fprintf(out,"          ret i32 ");
						fprintf(out,"%s\n",shuzi[0].name2.c_str());
					}
					else if(shuzi[0].type==1)
					{
						fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,shuzi[0].name2.c_str());
						sprintf(ch,"%%x%d",numb);
						shuzi[0].name2=ch;
						fprintf(out,"          ret i32 ");
						fprintf(out,"%s\n",shuzi[0].name2.c_str());
					}
					return 6;
				}
				else
				{
					printf("5\n");
					return 0;
				}
			}
			else
			{
				printf("6\n");
				num = j;
				return 0;
			}
		}
		else if(a==3)
		{
			int flag=0;
			int biao=0;
			int k;
			bool isshuzu=false;
			string varname=temp;
			
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]=="[")
			{
				isshuzu=true;
				num++;
				while(letter[num]=="block")
				{
					num++;
				}
				top1=-1;
				top2=-1;
				if(Exp(index)>0)
				{
					computeshuzi(index);
					yiwei=shuzi[0];
					while(letter[num]=="block")
					{
						num++;
					}
					if(letter[num]=="]")
					{
						num++;
						if(letter[num]=="[")//二维
						{
							num++;				
							while(letter[num]=="block")
							{
								num++;
							}
							top1=-1;
							top2=-1;
							if(Exp(index)>0)
							{
								computeshuzi(index);
								erwei=shuzi[0];
								while(letter[num]=="block")
								{
									num++;
								}
								if(letter[num]=="]")
								{
									num++;
								}
								else
								{
									return 0;
								}
							}
							else
							{
								return 0;
							}
						 } 
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 0;
				}
				while(letter[num]=="block")
				{
					num++;
				}	
			}
			if(isshuzu==false)
			{
				for(k=index;k>=0;k=identstable[k].outnum)
				{ 
					for(int i=0;i<=identstable[k].top;i++)
					{
						if(identstable[k].idents[i].name==varname)
						{
							if(identstable[k].idents[i].type==0)
								return 0;
							else
							{
								flag=1;
								biao=i;
								break;
							}
						}
					}
					if(flag==1)
						break;
				}
				if(flag==0)
				{
					return 0;
				}
				while(letter[num]=="block")
				{
					num++;
				}
				if(letter[num]=="=")
				{
					num++;
					while(letter[num]=="block")
					{
						num++;
					}
					top1=-1;
					top2=-1;
					if(Exp(index)>0)
					{
						while(letter[num]=="block")
						{
							num++;
						}
						while(top2!=-1)
						{
							operate(op[top2]);
							top2--;
						}
						if(letter[num]==";")
						{
							num++;
							if(shuzi[0].type==0)
							{
								fprintf(out,"          store i32 %d, i32* %s\n",shuzi[0].value,identstable[k].idents[biao].name2.c_str());
							}
							else if(shuzi[0].type==2)
							{
								fprintf(out,"          store i32 %s, i32* %s\n",shuzi[0].name2.c_str(),identstable[k].idents[biao].name2.c_str());
							}
							else if(shuzi[0].type==1)
							{
								fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,shuzi[0].name2.c_str());
								fprintf(out,"          store i32 %%x%d, i32* %s\n",numb,identstable[k].idents[biao].name2.c_str());
							}
							identstable[k].idents[biao].value=shuzi[0].value;
							return 1;
						}
						else
						{
							printf("7\n");
							return 0;
						}
					}
					else
					{
						printf("%s",temp.c_str());
						printf("8\n");
						top1=-1;
						top2=-1;
						num = j;
						return 0;
					}
				}
				else
				{
					printf("%d ",a);
					printf("9\n");
					num = j;
					return 0;
				}
			}
			else if(isshuzu==true)
			{
				shuzu keyshuzu;
				for(k=index;k>=0;k=identstable[k].outnum)
				{
					for(int i=0;i<identstable[k].shuzus.size();i++)
					{
						if(varname==identstable[k].shuzus[i].name)
						{
							flag=1;
							biao=i;
							keyshuzu=identstable[k].shuzus[i];
							break;
						}
					}
					if(flag==1)
					{
						break;
					}
				}
				top1=-1;
				top2=-1;
				if(yiwei.type!=10)
				{
					shuzi[++top1]=yiwei;
				}
				if(erwei.type!=10)
				{
					ident newident;
					newident.type=0;
					newident.value=keyshuzu.erweilength;
					shuzi[++top1]=newident;
					op[++top2]='*';
					operate(op[top2]);
					top2--;
					shuzi[++top1]=erwei;
					op[++top2]='+';
				}
				computeshuzi(index);
				address=shuzi[0];
				if(flag==0)
				{
					return 0;
				}
				while(letter[num]=="block")
				{
					num++;
				}
				if(letter[num]=="=")
				{
					num++; 
				}
				else
				{
					return 0;
				}
				while(letter[num]=="block")
				{
					num++;
				}
				numb++;
				char ch[50];
				sprintf(ch,"%%x%d",numb);
				basepoint = ch;
				int length=keyshuzu.length;		
				fprintf(out,"          %s = getelementptr [%d x i32],[%d x i32]* %s, i32 0, i32 0\n",basepoint.c_str(),length,length,keyshuzu.name2.c_str());
				if(address.type==2)
					fprintf(out,"          %%x%d = getelementptr i32,i32* %s, i32 %s\n",++numb,basepoint.c_str(),address.name2.c_str());
				else if(address.type==0)
					fprintf(out,"          %%x%d = getelementptr i32,i32* %s, i32 %d\n",++numb,basepoint.c_str(),address.value);
				else if(address.type==1)
				{
					fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,address.name2.c_str());
					sprintf(ch,"%%x%d",numb);
					fprintf(out,"          %%x%d = getelementptr i32,i32* %s, i32 %s\n",++numb,basepoint.c_str(),ch);
				}
				sprintf(ch,"%%x%d",numb);
				string fuzhi=ch;
				top1=-1;
				top2=-1;
				if(Exp(index)>0)
				{
					while(letter[num]=="block")
					{
						num++;
					}
					while(top2!=-1)
					{
						operate(op[top2]);
						top2--;
					}
					if(letter[num]==";")
					{
						num++;
						if(shuzi[0].type==0)
						{
							fprintf(out,"          store i32 %d, i32* %s\n",shuzi[0].value,fuzhi.c_str());
						}
						else if(shuzi[0].type==2)
						{
							fprintf(out,"          store i32 %s, i32* %s\n",shuzi[0].name2.c_str(),fuzhi.c_str());
						}
						else if(shuzi[0].type==1)
						{
							fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,shuzi[0].name2.c_str());
							fprintf(out,"          store i32 %%x%d, i32* %s\n",numb,fuzhi.c_str());
						}
						if(address.value!=-1)
							identstable[k].shuzus[biao].value[address.value]=shuzi[0].value;
						top1=-1;
						top2=-1;
						return 1;
					}
					else
					{
						return 0;
					}
				}
				
				fprintf(out,"          store i32 %d, i32* %%x%d\n",shuzi[0].value,numb);
				if(InitVal(index)>0)
				{
					return 2;
				}
				else
				{
					return 0;
				}
			}
		}
		else if(a==20)
		{
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]=="(")
			{
				num++;
				while(letter[num]=="block")
				{
					num++;
				}		
				if(Cond(index)>0)
				{
					while(letter[num]=="block")
					{
						num++;
					}
					if(letter[num]==")")
					{
						num++;
						while(letter[num]=="block")
						{
							num++;
						}
						int if_block=++blocknum;
						int out_block=++blocknum;
						if(shuzi[0].type==2)
						{
							fprintf(out,"          %%x%d = icmp ne i32 %s, 0\n",++numb,shuzi[0].name2.c_str());
							sprintf(ch,"%%x%d",numb);
							shuzi[0].name2=ch;
							shuzi[0].type=3;
						}
						else if(shuzi[0].type==1)
						{
							fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,address.name2.c_str());
							sprintf(ch,"%%x%d",numb);
							string tempstring = ch;
							fprintf(out,"          %%x%d = icmp ne i32 %s, 0\n",++numb,tempstring.c_str());
							shuzi[0].name2=ch;
							shuzi[0].type=3;
						}
						fprintf(out,"          br i1 %s ,label %%basic_block_%d, label %%basic_block_%d\n",shuzi[0].name2.c_str(),if_block,out_block);
						fprintf(out,"\n");
						fprintf(out,"          basic_block_%d:\n",if_block);
						if(Stmt(index)>0)
						{
							while(letter[num]=="block")
							{
								num++;
							}
							int x=num;
							int b = judgeword(letter[num],num);
							{
								if(b==21)
								{
									while(letter[num]=="block")
									{
										num++;
									}
									int else_block=out_block;
									out_block=++blocknum;
									fprintf(out,"          br label %%basic_block_%d\n",out_block);
									fprintf(out,"\n");
									fprintf(out,"          basic_block_%d:\n",else_block);
									if(Stmt(index)>0)
									{
										fprintf(out,"          br label %%basic_block_%d\n",out_block);
										fprintf(out,"\n");
										fprintf(out,"          basic_block_%d:\n",out_block);
										return 4;
									}
									else
									{
										return 0;
									}
								}
								else
								{
									fprintf(out,"          br label %%basic_block_%d\n",out_block);
									fprintf(out,"\n");
									fprintf(out,"          basic_block_%d:\n",out_block);
									num=x;
									return 4;
								}
							}		
						}
						else
						{
							return 0;
						}
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else if(a==22)
		{
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]=="(")
			{
				num++;
				while(letter[num]=="block")
				{
					num++;
				}		
				int judge_block=++blocknum;
				judge_blocks.push(judge_block);
				fprintf(out,"          br label %%basic_block_%d\n",judge_block);
				fprintf(out,"\n");
				fprintf(out,"          basic_block_%d:\n",judge_block);
				if(Cond(index)>0)
				{
					while(letter[num]=="block")
					{
						num++;
					}
					if(letter[num]==")")
					{
						num++;
						while(letter[num]=="block")
						{
							num++;
						}
						int in_block=++blocknum;
						int out_block=++blocknum;
						out_blocks.push(out_block);
						if(shuzi[0].type==2)
						{
							fprintf(out,"          %%x%d = icmp ne i32 %s, 0\n",++numb,shuzi[0].name2.c_str());
							sprintf(ch,"%%x%d",numb);
							shuzi[0].name2=ch;
							shuzi[0].type=3;
						}
						else if(shuzi[0].type==0)
						{
							fprintf(out,"          %%x%d = icmp ne i32 %d, 0\n",++numb,shuzi[0].value);
							sprintf(ch,"%%x%d",numb);
							shuzi[0].name2=ch;
							shuzi[0].type=3;
						}
						else if(shuzi[0].type==1)
						{
							fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,address.name2.c_str());
							sprintf(ch,"%%x%d",numb);
							string tempstring = ch;
							fprintf(out,"          %%x%d = icmp ne i32 %s, 0\n",++numb,tempstring.c_str());
							shuzi[0].name2=ch;
							shuzi[0].type=3;
						}
						fprintf(out,"          br i1 %s ,label %%basic_block_%d, label %%basic_block_%d\n",shuzi[0].name2.c_str(),in_block,out_block);
						fprintf(out,"\n");
						fprintf(out,"          basic_block_%d:\n",in_block);
						if(Stmt(index)>0)
						{
							while(letter[num]=="block")
							{
								num++;
							}
							fprintf(out,"          br label %%basic_block_%d\n",judge_block);
							fprintf(out,"\n");
							fprintf(out,"          basic_block_%d:\n",out_block);
							judge_blocks.pop();
							out_blocks.pop();
							return 5;	
						}
						else
						{
							return 0;
						}
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else if(a==23)
		{
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]==";")
			{
				num++;
				fprintf(out,"          br label %%basic_block_%d\n",judge_blocks.top()); 
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else if(a==24)
		{
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]==";")
			{
				num++;
				fprintf(out,"          br label %%basic_block_%d\n",out_blocks.top()); 
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			printf("%d ",a);
			printf("%s ",temp.c_str());
			printf("10\n");
			num=j;
			return 0;
		}
	}
	num=j;
	printf("11\n");
	return 0;
}
int Number(string s,int n)
{
	stringstream ss1;
	int j=n+1;
	int ret;
	if((s=="0"&&letter[j]=="x"||s=="0"&&letter[j]=="X")&&letter[j]!="block")//十六进制 
	{
		s=(s+letter[j]).c_str();
 		j++;
 		while(1)
 		{
 			if(ishexdigit(letter[j]))
 			{
 				s=(s+letter[j]).c_str();
   				j++;
			}
			else
			{
				num=j;
				ss1.str(s);
				ss1>>hex>>ret;
				return ret;
			}
		}
	}
	else if(s=="0"&&letter[j]!="x"&&letter[j]!="X")//八进制 
	{
		while(1)
		{
			if(isoctaldigit(letter[j]))
			{
				s=(s+letter[j]).c_str();
   				j++;
			}
			else
			{
				num=j;
				ss1.str(s);
				ss1>>oct>>ret;
				return ret;
			}
		}
	}
	else if(isnonezero(s))
	{
		while(1)
		{
			if(letter[j]>="0"&&letter[j]<="9")
			{
				s=(s+letter[j]).c_str();
   				j++;
			}
			else
			{
				num=j;
				ss1.str(s);
				ss1>>ret;
				return ret;
			}
		}
	}
 num=j;
 return -1;
}
int Exp(int index)
{	
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if(AddExp(index)>0)
	{
		return 1;
	}
	else
	{
		num=j;
		return 0;
	}
 } 
int AddExp(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if(MulExp(index)>0)
	{
		while(letter[num]=="block")
		{
			num++;
		}
		while(letter[num]=="+"||letter[num]=="-")
		{
			if(index>0)
			{
				while(op[top2]!='('&&top2!=-1)
				{
					operate(op[top2]);
					top2--;
				}
			}
			else
			{
				while(op[top2]!='('&&top2!=-1)
				{
					operatewithnoprint(op[top2]);
					top2--;
				}
			}
			if(letter[num]=="+") 
				op[++top2]='+';
			else if(letter[num]=="-")
				op[++top2]='-';
			num++;
			while(letter[num]=="block")
			{
				num++;
			}
			if(MulExp(index)==0)
			{
				return 0;
			}
			while(letter[num]=="block")
			{
				num++;
			}
		}
		return 1;
	}
	else
	{
		num=j;
		return 0;
	}
}
int MulExp(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if(UnaryExp(index)>0)
	{
		while(letter[num]=="block")
		{
			num++;
		}
//		if(letter[num]=="=")
//			return 0;
		while(letter[num]=="*"||letter[num]=="/"||letter[num]=="%")
		{
			
			if(op[top2]=='+'||op[top2]=='-'||op[top2]=='('||top2==-1)
			{
				if(letter[num]=="*")
					op[++top2]='*';
				else if(letter[num]=="/")
					op[++top2]='/';
				else if(letter[num]=="%")
					op[++top2]='%';				
			}
			else if(op[top2]=='*'||op[top2]=='%'||op[top2]=='/')
			{
				if(index>0)
				{
					while(op[top2]=='*'||op[top2]=='%'||op[top2]=='/')
					{
						operate(op[top2]);
						top2--;
					}
				}
				else
				{
					while(op[top2]=='*'||op[top2]=='%'||op[top2]=='/')
					{
						operatewithnoprint(op[top2]);
						top2--;
					}
				}
				if(letter[num]=="*")
					op[++top2]='*';
				else if(letter[num]=="/")
					op[++top2]='/';
				else if(letter[num]=="%")
					op[++top2]='%';
			}
			num++;
			while(letter[num]=="block")
			{
				num++;
			}
			if(UnaryExp(index)==0)
				return 0;
			while(letter[num]=="block")
			{
				num++;
			}
		}
		while(letter[num]=="block")
		{
			num++;
		}
		return 1;
	}
	else
	{
		num=j;
		return 0;
	}
 } 
int PrimaryExp(int opt,int numfei,int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int x=num;
	if(letter[num]=="(")
	{
		op[++top2]='(';
		num++;
		while(letter[num]=="block")
		{
			num++;
		}
		if(Exp(index)>0)
		{
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]==")")
			{
				while(op[top2]!='(')
				{
					operate(op[top2]);
					top2--;
				}
				top2--;
				if(opt==-1)
				{
				
					op[++top2]='(';
					ident newident=shuzi[top1];
					shuzi[++top1]=newident;
					shuzi[top1-1].type=0;
					shuzi[top1-1].value=0;
					op[++top2]='-';
					if(index>0)
					{
						while(op[top2]!='(')
						{
							operate(op[top2]);
							top2--;
						}
					}
					else
					{
						while(op[top2]!='(')
						{
							operatewithnoprint(op[top2]);
							top2--;
						}
					}
					top2--;
				}
				num++;
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			num=x;
			return 0;
		}
		
	}
	else if(letter[num]>="a"&&letter[num]<="z"||letter[num]>="A"&&letter[num]<="Z"||letter[num]=="_")//把对应的数字入栈 
	{
		string s = letter[num];
		if(judgeword(s,num)==3)
		{
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]=="[")//是数组 
			{
				ident yiwei;
				ident erwei;
				yiwei.type=10;
				erwei.type=10;
				ident address;
				int flag=0;
				shuzu newshuzu;
				for(int k=index;k>=0;k=identstable[k].outnum)
				{
					for(int i=0;i<identstable[k].shuzus.size();i++)
					{
						if(identstable[k].shuzus[i].name==temp)
						{
							newshuzu=identstable[k].shuzus[i];
							flag=1;
							break;
						}
					}
					if(flag==1)
					{
						break;
					}
				}
				if(newshuzu.isconst==0&&constdef==true)
				{
					return 0;
				}
				if(flag==0)
				{
					return 0;
				}
				num++;
				while(letter[num]=="block")
				{
					num++;
				}
				op[++top2]='(';		
				if(Exp(index)>0)
				{
					while(op[top2]!='(')
					{
						operate(op[top2]);
						top2--;
					}
					//computeshuzi(index);
					ident yiwei=shuzi[top1];
//					if(newshuzu.type==2)
//						if(yiwei.type==0)
//							address=yiwei*newshuzu.erweilength;
//					else
//						if(yiwei.type==0)
//							address=yiwei;
					while(letter[num]=="block")
					{
						num++;
					}
					if(letter[num]=="]")
					{
						num++;
						while(letter[num]=="block")
						{
							num++;
						}
						if(letter[num]=="[")//如果是二维数组 
						{
							if(newshuzu.type==1)
								return 0;
							ident tempident;
							tempident.type=0;
							tempident.value=newshuzu.erweilength;
							shuzi[++top1]=tempident;
							op[++top2]='*';
							operate(op[top2]);
							top2--;
							op[++top2]='+';
							num++;
							while(letter[num]=="block")
							{
								num++;
							}
							op[++top2]='(';	
							if(Exp(index)>0)
							{
								while(op[top2]!='(')
								{
									operate(op[top2]);
									top2--;
								}
								top2--;
								while(op[top2]!='(')
								{
									operate(op[top2]);
									top2--;
								}
							}
							while(letter[num]=="block")
							{
								num++;
							}
							if(letter[num]=="]")
							{
								num++;
							}
							else
							{
								return 0;
							}
							while(letter[num]=="block")
							{
								num++;
							}
						}
						address=shuzi[top1];
						top1--;
						top2--;
						while(letter[num]=="block")
						{
							num++;
						}
						fprintf(out,"          %%x%d = getelementptr [%d x i32],[%d x i32]* %s, i32 0, i32 0\n",++numb,newshuzu.length,newshuzu.length,newshuzu.name2.c_str());
						char ch[50];
						string tempstring;
						sprintf(ch,"%%x%d",numb);
						tempstring = ch;
						if(address.type==0)
						{
							fprintf(out,"          %%x%d = getelementptr i32,i32* %s, i32 %d\n",++numb,tempstring.c_str(),address.value);
						}
						else if(address.type==2)
						{
							fprintf(out,"          %%x%d = getelementptr i32,i32* %s, i32 %s\n",++numb,tempstring.c_str(),address.name2.c_str());
						}
						else if(address.type==1)
						{
							fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,address.name2.c_str());
							sprintf(ch,"%%x%d",numb);
							address.name2 = ch;
							fprintf(out,"          %%x%d = getelementptr i32,i32* %s, i32 %s\n",++numb,tempstring.c_str(),address.name2.c_str());
						}
						sprintf(ch,"%%x%d",numb);
						tempstring = ch;
						fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,tempstring.c_str());
						sprintf(ch,"%%x%d",numb);
						tempstring = ch;
						ident newident;
						newident.name2=tempstring;
						if(address.type==0)
							newident.value=newshuzu.value[address.value];
						newident.name="";
						newident.type=2;
						shuzi[++top1]=newident;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 0;
				}
				while(letter[num]=="block")
				{
					num++;
				}	
			}
			else
			{
				int flag=0;
				int top3=identstable[index].top;
				for(int k=index;k>=0;k=identstable[k].outnum)
				{
					for(int i=0;i<=identstable[k].top;i++)
					{
						if(identstable[k].idents[i].name==temp)
						{
							if(identstable[k].idents[i].type==1&&constdef==true)
							{
								return 0;
							}
							ident newident1=identstable[k].idents[i],newident2;
							if(numfei>0)
							{
								char ch[10];
								if(numfei>0)
								{
									if(newident1.type==1)
									{
										fprintf(out,"          %%x%d = load i32, i32* %s\n",++numb,newident1.name2.c_str());
										sprintf(ch,"%%x%d",numb);
										newident1.name2=ch;
										newident1.type=2; 
									}
								 } 
								while(numfei>0)
								{
									fprintf(out,"          %%x%d = icmp eq i32 %s, 0\n",++numb,newident1.name2.c_str());
									sprintf(ch,"%%x%d",numb);
									newident1.name2=ch;
									newident1.type=3;
									fprintf(out,"          %%x%d = zext i1 %s to i32\n",++numb,newident1.name2.c_str());
									sprintf(ch,"%%x%d",numb);
									newident1.name2=ch;
									newident1.type=2;
									numfei--;
								}
							}
							if(opt==-1)
							{
								op[++top2]='(';
								newident2.type=0;
								newident2.value=0;
								newident2.name="";
								newident2.name2="";
								shuzi[++top1]=newident2;//0
								op[++top2]='-';
								shuzi[++top1]=newident1;
								if(index>0)
								{
									while(op[top2]!='(')
									{
										operate(op[top2]);
										top2--;
									}
								}
								else
								{
									while(op[top2]!='(')
									{
										operatewithnoprint(op[top2]);
										top2--;
									}
								}
								top2--;
							}
							else if(opt==1)
							{
								shuzi[++top1]=newident1;
							}
							flag=1;
							break;
						}
					}
					if(flag==1)
						break;
				}
				if(flag==0)
				{
					return 0;
				}
			}
		}
		else
		{
			num=x;
			return 0;
		}
	}
	else if(letter[num]>="0"&&letter[num]<="9")//number
	{
		int j=num;
		string s=letter[j];
		result=Number(s,j);
		ident newident;
		newident.value=opt*result;
		newident.name="";
		newident.name2="";
		newident.type=0;
		shuzi[++top1]=newident;
		if(result!=-100862)
		{
			return 2;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	return 1;
}
int UnaryExp(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if(letter[num]>="a"&&letter[num]<="z"||letter[num]>="A"&&letter[num]<="Z"||letter[num]=="_")
	{
		int a = judgeword(letter[num],num);
		if(a>=10&&a<=15)
		{
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]=="(")
			{
				num++;
				while(letter[num]=="block")
				{
					num++;
				}
				if(a==10&&letter[num]==")")
				{
					fprintf(out,"          %%x%d = call i32 @getint()\n",++numb);
					char ch[50];
					sprintf(ch,"%%x%d",numb);
					ident newident;
					newident.value=0;
					newident.name="";
					newident.name2=ch;
					newident.type=2;
					shuzi[++top1]=newident;
					num++;
					return 2;
				}
				else if(a==11&&letter[num]==")")
				{
					fprintf(out,"          %%x%d = call i32 @getch()\n",++numb);
					char ch[50];
					sprintf(ch,"%%x%d",numb);
					ident newident;
					newident.value=0;
					newident.name="";
					newident.name2=ch;
					newident.type=2;
					shuzi[++top1]=newident;
					num++;
					return 2;
				}
				else if(a==13)
				{
					if(FuncRParams(index)>0)
					{
						if(shuzi[0].type==1)
						{
							fprintf(out,"          %%x%d = load i32, i32* %2s\n",++numb,shuzi[0].name2.c_str());
							fprintf(out,"          call void @putint(i32 %%x%d)\n",numb);
						}
						else if(shuzi[0].type==0)
							fprintf(out,"          call void @putint(i32 %d)\n",shuzi[0].value);
						else if(shuzi[0].type==2)
							fprintf(out,"          call void @putint(i32 %s)\n",shuzi[0].name2.c_str());
						num++;
						return 2;
					}
					else
					{
						return 0;
					}
					
				}
				else if(a==14)
				{
					if(FuncRParams(index)>0)
					{
						if(shuzi[0].type==1)
						{
							fprintf(out,"          %%x%d = load i32, i32* %2s\n",++numb,shuzi[0].name2.c_str());
							fprintf(out,"          call void @putch(i32 %%x%d)\n",numb);
						}
						else if(shuzi[0].type==0)
							fprintf(out,"          call void @putch(i32 %d)\n",shuzi[0].value);
						else if(shuzi[0].type==2)
							fprintf(out,"          call void @putch(i32 %s)\n",shuzi[0].name2.c_str());
						num++;
						return 2;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			num=j;
		}
	}
	else
	{
		num=j;
	}
	int opt=1;
	int numfei=0;
	while(letter[num]=="+"||letter[num]=="-"||letter[num]=="!")
	{
		if(letter[num]=="-")
		{
			opt = -opt;
		}
		else if(letter[num]=="!")
		{
			numfei++;
		}
		num++;
		while(letter[num]=="block")
		{
			num++;
		}
	}
	if(PrimaryExp(opt,numfei,index)>0)
		return 1;
	else
		return 0;
}
int RelExp(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if(AddExp(index)>0)
	{
		while(letter[num]=="block")
		{
			num++;
		}
		while(letter[num]==">"||letter[num]=="<")
		{
			if(index>0)
			{
				while(top2!=-1)
				{
					operate(op[top2]);
					top2--;
				}
			}
			else
			{
				while(top2!=-1)
				{
					operatewithnoprint(op[top2]);
					top2--;
				}
			}
			int judge;
			char ch[10];
			ident yuan=shuzi[0];
			top1=-1;
			top2=-1;
			if(letter[num]==">")
			{
				judge=1;
			}
			else if(letter[num]=="<")
			{
				judge=2;
			}
			num++;
			if(letter[num]=="=")
			{
				judge+=2;
				num++;
			}
			while(letter[num]=="block")
			{
				num++;
			}
			if(AddExp(index)<=0)
			{
				return 0;
			}
			while(top2!=-1)
			{
				operate(op[top2]);
				top2--;
			}
			while(letter[num]=="block")
			{
				num++;
			}
			PrintRel(judge,yuan);
		}
		return 1;
	}
	else
	{
		return 0;
	}
	return 1;
}
int EqExp(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if(RelExp(index)>0)
	{
		while(letter[num]=="block")
		{
			num++;
		}
		while((letter[num]=="="||letter[num]=="!")&&letter[num+1]=="=")
		{
			int judge=1;
			if(index>0)
			{
				while(top2!=-1)
				{
					operate(op[top2]);
					top2--;
				}
			}
			else
			{
				while(top2!=-1)
				{
					operatewithnoprint(op[top2]);
					top2--;
				}
			}
			char ch[10];
			ident yuan=shuzi[0];
			top1=-1;
			top2=-1;
			if(letter[num]=="=")
			{
				judge=1;
			}
			else if(letter[num]=="!")
			{
				judge=2;
			}
			num+=2;
			while(letter[num]=="block")
			{
				num++;
			}
			if(RelExp(index)<=0)
			{
				return 0;
			}
			while(top2!=-1)
			{
				operate(op[top2]);
				top2--;
			}
			while(letter[num]=="block")
			{
				num++;
				
			}
			PrintEq(judge,yuan);
		}
		return 1;
	}
	else
	{
		return 0;
	}
	return 1;
}
int LAndExp(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if(EqExp(index)>0)
	{
		while(letter[num]=="block")
		{
			num++;
		}
		while(letter[num]=="&"&&letter[num+1]=="&")
		{
			if(index>0)
			{
				while(top2!=-1)
				{
					operate(op[top2]);
					top2--;
				}
			}
			else
			{
				while(top2!=-1)
				{
					operatewithnoprint(op[top2]);
					top2--;
				}
			}
			char ch[10];
			ident yuan=shuzi[0];
			top1=-1;
			top2=-1;
			num+=2;
			while(letter[num]=="block")
			{
				num++;
			}
			if(EqExp(index)<=0)
			{
				return 0;
			}
			while(top2!=-1)
			{
				operate(op[top2]);
				top2--;
			}
			while(letter[num]=="block")
			{
				num++;
			}
			PrintLAnd(yuan);
		}
		return 1;
	}
	else
	{
		return 0;
	}
	return 1;
}
int LOrExp(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	if(LAndExp(index)>0)
	{
		while(letter[num]=="block")
		{
			num++;
		}
		while(letter[num]=="|"&&letter[num+1]=="|")
		{
			char ch[10];
			ident yuan=shuzi[0];
			top1=-1;
			top2=-1;
			num+=2;
			while(letter[num]=="block")
			{
				num++;
			}
			if(LAndExp(index)<=0)
			{
				return 0;
			}
			if(index>0)
			{
				while(top2!=-1)
				{
					operate(op[top2]);
					top2--;
				}
			}
			else
			{
				while(top2!=-1)
				{
					operatewithnoprint(op[top2]);
					top2--;
				}
			}
			while(letter[num]=="block")
			{
				num++;
			}
			PrintLOr(yuan);
		}
		return 1;
	}
	else
	{
		return 0;
	}
	return 1;
}
int Cond(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	if(LOrExp(index)>0)
	{
		return 1;
	}
	return 0;
}
int FuncRParams(int index)
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if(Exp(index)>0)
	{
		if(index>0)
		{
			while(top2!=-1)
			{
				operate(op[top2]);
				top2--;
			}
		}
		else
		{
			while(top2!=-1)
			{
				operatewithnoprint(op[top2]);
				top2--;
			}
		}
		while(letter[num]=="block")
		{
			num++;
		}
		while(letter[num]==",")
		{
			num++;
			if(Exp(index)>0)
			{
				computeshuzi(index);
				while(letter[num]=="block")
				{
					num++;
				}
			}
			else
			{
				return 0;
			}
		}
		return 1;
	}
	else
	{
		num=j;
		return 0;
	}
}
int quanjuDecl()
{
	if((letter[num]>="a"&&letter[num]<="z")||(letter[num]>="A"&&letter[num]<="Z")||letter[num]=="_")
	{
		int a = judgeword(letter[num],num);
		if(a==7)//const
		{
			while(letter[num]=="block")
			{
				num++;
			}
			int b = judgeword(letter[num],num);
			while(letter[num]=="block")
			{
				num++;
			}
			if(b==1)
			{		
				int j=num;
				if(ConstDef(0)>0)
				{
					if(isshuzudef==true)
					{
						fprintf(out,"          %s = dso_local constant [%d x i32] [",identstable[0].shuzus.back().name2.c_str(),identstable[0].shuzus.back().length);
						for(int i=0;i<identstable[0].shuzus.back().length;i++)
						{
							if(i==0)
							{
								fprintf(out,"i32 %d",identstable[0].shuzus.back().value[i]);
							}
							else
							{
								fprintf(out,", i32 %d",identstable[0].shuzus.back().value[i]);
							}
						}
						identstable[0].shuzus.back().isconst=1;
						fprintf(out,"]\n");
						isshuzudef=false;
					}
					while(letter[num]=="block")
					{
						num++;
					}
				}
				else
				{
					while(letter[num]=="block")
					{
						num++;
					}
					if(isshuzudef==true)
					{
						fprintf(out,"          %s = dso_local constant [%d x i32] zeroinitializer\n",identstable[0].shuzus.back().name2.c_str(),identstable[0].shuzus.back().length);
						isshuzudef=false;
						for(int j=0;j<identstable[0].shuzus.back().length;j++)
						{
							identstable[0].shuzus.back().value.push_back(0);
						}
						identstable[0].shuzus.back().isconst=1;
					}
					while(letter[num]=="block")
					{
						num++;
					}			
				}
				while(letter[num]==",")
				{
					num++;
					int j=num;
					if(ConstDef(0)>0)
					{
						if(isshuzudef==true)
						{
							fprintf(out,"          %s = dso_local constant [%d x i32] [",identstable[0].shuzus.back().name2.c_str(),identstable[0].shuzus.back().length);
							identstable[0].shuzus.back().isconst=1;
							for(int i=0;i<identstable[0].shuzus.back().length;i++)
							{
								if(i==0)
								{
									fprintf(out,"i32 %d",identstable[0].shuzus.back().value[i]);
								}
								else
								{
									fprintf(out,", i32 %d",identstable[0].shuzus.back().value[i]);
								}
							}
							fprintf(out,"]\n");
							isshuzudef=false;
						}
						while(letter[num]=="block")
						{
							num++;
						}
					}
					else
					{
						while(letter[num]=="block")
						{
							num++;
						}
						if(isshuzudef==true)
						{
							identstable[0].shuzus.back().isconst=1;
							fprintf(out,"          %s = dso_local constant [%d x i32] zeroinitializer\n",identstable[0].shuzus.back().name2.c_str(),identstable[0].shuzus.back().length);
							isshuzudef=false;
							for(int j=0;j<identstable[0].shuzus.back().length;j++)
							{
								identstable[0].shuzus.back().value.push_back(0);
							}
						}
						while(letter[num]=="block")
						{
							num++;
						}
					}
					
				}
				while(letter[num]=="block")
				{
					num++;
				}
				if(letter[num]==";")
				{
					num++;
					return 1;
				}
				else
				{
					return 0;
				}
					
			}
			else
			{
				return 0;
			}
				
		}
		else if(a==1)
		{
			while(letter[num]=="block")
			{
				num++;
			}
			int j=num;	
			if(ConstDef(0)>0)
			{
				if(isshuzudef==true)
				{
					identstable[0].shuzus.back().isconst=0;
					fprintf(out,"          %s = dso_local global [%d x i32] [",identstable[0].shuzus.back().name2.c_str(),identstable[0].shuzus.back().length);
					for(int i=0;i<identstable[0].shuzus.back().length;i++)
					{
						if(i==0)
						{
							fprintf(out,"i32 %d",identstable[0].shuzus.back().value[i]);
						}
						else
						{
							fprintf(out,", i32 %d",identstable[0].shuzus.back().value[i]);
						}
					}
					fprintf(out,"]\n");
					isshuzudef=false;
				}
				else
				{
					identstable[0].idents[identstable[0].top].type=1;
					char ch[50];
					sprintf(ch,"@%s",identstable[0].idents[identstable[0].top].name.c_str());
					identstable[0].idents[identstable[0].top].name2=ch;
					fprintf(out,"%s = dso_local global i32 %d\n",identstable[0].idents[identstable[0].top].name2.c_str(),shuzi[0].value);
				}
				while(letter[num]=="block")
				{
					num++;
				}
			}
			else
			{

				while(letter[num]=="block")
				{
					num++;
				}
				if(ismain==true)
				{
					ismain=false;
					return 0;
				}
				if(isshuzudef==true)
				{
					identstable[0].shuzus.back().isconst=0;
					fprintf(out,"          %s = dso_local global [%d x i32] zeroinitializer\n",identstable[0].shuzus.back().name2.c_str(),identstable[0].shuzus.back().length);
					isshuzudef=false;
					for(int j=0;j<identstable[0].shuzus.back().length;j++)
					{
						identstable[0].shuzus.back().value.push_back(0);
					}
				}
				else
				{
					identstable[0].idents[identstable[0].top].type=1;
					char ch[50];
					sprintf(ch,"@%s",identstable[0].idents[identstable[0].top].name.c_str());
					identstable[0].idents[identstable[0].top].name2=ch;
					fprintf(out,"%s = dso_local global i32 0\n",identstable[0].idents[identstable[0].top].name2.c_str());
				}
				while(letter[num]=="block")
				{
					num++;
				}
			}
			while(letter[num]==",")
			{
				num++;
				int j=num;
				int x = ConstDef(0);
				if(x>0)
				{
					if(isshuzudef==true)
					{
						identstable[0].shuzus.back().isconst=0;
						fprintf(out,"          %s = dso_local global [%d x i32] [",identstable[0].shuzus.back().name2.c_str(),identstable[0].shuzus.back().length);
						for(int i=0;i<identstable[0].shuzus.back().length;i++)
						{
							if(i==0)
							{
								fprintf(out,"i32 %d",identstable[0].shuzus.back().value[i]);
							}
							else
							{
								fprintf(out,", i32 %d",identstable[0].shuzus.back().value[i]);
							}
						}
						fprintf(out,"]\n");
						isshuzudef=false;
						while(letter[num]=="block")
						{
							num++;
						}
					}
					else
					{
						identstable[0].idents[identstable[0].top].type=1;
						char ch[50];
						sprintf(ch,"@%s",identstable[0].idents[identstable[0].top].name.c_str());
						identstable[0].idents[identstable[0].top].name2=ch;
						fprintf(out,"%s = dso_local global i32 %d\n",identstable[0].idents[identstable[0].top].name2.c_str(),shuzi[0].value);
					}	
				}
				else
				{
					while(letter[num]=="block")
					{
						num++;
					}
					if(isshuzudef=true)
					{
						identstable[0].shuzus.back().isconst=0;
						fprintf(out,"          %s = dso_local global [%d x i32] zeroinitializer\n",identstable[0].shuzus.back().name2.c_str(),identstable[0].shuzus.back().length);
						isshuzudef=false;
						for(int j=0;j<identstable[0].shuzus.back().length;j++)
						{
							identstable[0].shuzus.back().value.push_back(0);
						}
					}
					else
					{
						identstable[0].idents[identstable[0].top].type=1;
						char ch[50];
						sprintf(ch,"@%s",identstable[0].idents[identstable[0].top].name.c_str());
						identstable[0].idents[identstable[0].top].name2=ch;
						fprintf(out,"%s = dso_local global i32 0\n",identstable[0].idents[identstable[0].top].name2.c_str());
					}
					while(letter[num]=="block")
					{
						num++;
					}
				}
			}
			while(letter[num]=="block")
			{
				num++;
			}
			if(letter[num]==";")
			{
				num++;
				return 1;
			}
			else
			{
				return 0;
			}
					
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
int FuncDef()
{
	while(letter[num]=="block")
	{
		num++;
	}
	int j=num;
	if(quanjuDecl()>0)
	{
		j=num;
		while(letter[num]=="block")
		{
			num++;
		}
		while(quanjuDecl()>0)
		{
			
			while(letter[num]=="block")
			{
				num++;
			}
			j=num;
		}
		num=j;
	}
	else
	{
		num=j;
	}
	while(letter[num]=="block")
	{
		num++;
	}
	if(judgeword(letter[num],num)==1)
	{
		fprintf(out,"define dso_local i32 ");
		while(letter[num]=="block")
		{
			num++;
		}
		if(judgeword(letter[num],num)==2)
		{
			fprintf(out,"@main");
			while(letter[num]=="block")
				num++;
			if(symbol(letter[num])==3)
			{
				while(letter[num]=="block")
					num++;
				if(symbol(letter[num])==4)
				{
					while(letter[num]=="block")
						num++;
					if(symbol(letter[num])==5)
					{
						while(letter[num]=="block")// {
							num++;
						blocknum++;
						identstable[1].top=0;
						identstable[1].outnum=0;
						while(Blockitem(1)>0)//return
						{
							while(letter[num]=="block")
								num++;	
						}
						while(letter[num]=="block")
							num++;
						if(symbol(letter[num])==9)
						{
							return 1;
						}
						else
						{
							return 0;
						 } 
					}
				}
			}	
		}
	}
	return 0;
}
int main(int argc,char **argv){
	char w;

	in = fopen(argv[1],"r");
	out = fopen(argv[2],"w");
 	int flag=0;
	
	fprintf(out,"declare i32 @getint()\n");
	fprintf(out,"declare void @putint(i32)\n");
	fprintf(out,"declare i32 @getch()\n");
	fprintf(out,"declare void @putch(i32)\n");
	identstable[0].outnum=-1; 
	while(fgets(line,1000,in)!=NULL)
 	{
 		int len=strlen(line);
	 	for(int k=0;line[k]>0;k++)
		{
			if(line[k]==47&&line[k+1]==42)//是/*形注释 
			{
				flag=1;
			} 
			if(flag==1)
			{
				if(k>0)
				{
					if(line[k-1]==42&&line[k]==47)
					{
						flag=0;
					}
				}
				continue;
			}
			if(k>=len)
			{
				break;
			 } 
			if(line[k]==47&&line[k+1]==47)//是//形注释 
			{
				break;
			}
			w=line[k];
  			if(!isblank(w)&&w!='\n'&&w!='\r')
			{
   				letter[length]=w;
   				length++;
  			} 
  			else if(isblank(w)||w=='\n'&&w!='\r')
  			{
  				letter[length]="block";
  				length++;
  				if(w=='\n')
  				{
  					break;
				}
  			}
  			else
  			{
  				break;
  			}
  		}
	}
	int fd=FuncDef();
	if(fd>0)
	{
		printf("可以return\n");
		return 0;
	}
	else
	{
		return 1;
	}
}
