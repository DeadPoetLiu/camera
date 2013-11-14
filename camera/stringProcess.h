#ifndef STRINGPROCESS_H
#define  STRINGPROCESS_H
#include "headers.h"
using namespace std;
class ImgPath{
public:std::string path;
	int last;
	
public:
	
	   void setPath(string acts,string p){
		   cout<<"acts:"<<acts<<endl;
		   cout<<"p"<<p<<endl;

		   if(p[0]=='.'){
			 cout<<"sub"<<acts.substr(0,findLast(acts,'/'))<<endl;
			path=acts.substr(0,findLast(acts,'/'))+p.substr(1,p.size());
		  }else{
			path=p;
			cout<<"path"<<path<<endl;
		  }

		last=findLast(path,'.');
		 cout<<"path:"<<path<<endl;
	   }
	

public:	std::string  get(int num){
	std::string img=path;
   

	std::stringstream ss;
	ss<<num;
	std::string ns=ss.str();
	for(int i=ns.size()-1;i>=0;i--)
		img[--last]=ns[i];
//	cout<<path<<endl;
	return img;
    }

private:	int findLast(const std::string & path,char c){
	 int last=-1;
	for(int i=path.size()-1;i>=0;i--){
		if(path[i]==c){
			last=i;
			break;
		}
	}
	return last;
   }

};
 
#endif





