#ifndef ACT_H
#define ACT_H
#include "utility.h"
#include "headers.h"
#include "stringProcess.h"
class Act{
public:
	ImgPath ip;
	std::string imgPath;
	int start;
	int end;
	int step;
	std::string fileName;
	std::ifstream in;
	double focal,xp,yp;
	double scale;
	osg::ref_ptr<osg::Vec3Array> points;
	osg::ref_ptr<osg::Vec3Array> cameraCenters;
	osg::ref_ptr<osg::Group> group;
	Act():scale(10){

	}

	void set(string actFile){
		points=new osg::Vec3Array;
		cameraCenters=new osg::Vec3Array;
		group=new osg::Group;

		fileName=actFile;
		in.open(fileName);
		if(!in)
			cout<<"error can't open file"<<endl;
	
		
		readImgPath();
	    readIntrinsic();
		in.close();
		ip.setPath(fileName,imgPath);
		output();
	}
   
	void output(){
		cout<<"ippath:"<<ip.path<<" "<<endl;
		cout<<"imgpath"<<this->imgPath<<endl;
		cout<<"filename"<<this->fileName<<endl;
		cout<<start<<" "<<end<<" "<<step<<" "<<endl;

	}
	void readImgPath(){
		std::string s;
		while(std::getline(in,s)&&s!=std::string("<Image Sequence>"));
		std::string temp;

		
		std::getline(in,imgPath);
		imgPath=imgPath.substr(9,imgPath.size());

		std::getline(in,s);
		temp=s.substr(6,s.size());
		start= std::stoi(temp);
		//cout<<start<<endl;
		std::getline(in,s);
		temp=s.substr(5,s.size());
		step= std::stoi(temp);
		//	cout<<step<<endl;
		std::getline(in,s);
		temp=s.substr(4,s.size());
		end= std::stoi(temp);

	}
	void readIntrinsic(){
		
		string s;
		while(std::getline(in,s)&&s!=std::string("<intrinsic parameter>"));
		double a,b,c,d,e,f;
		in>>a>>b>>c>>d;

		focal=a;
		xp=c;
		yp=d;

		while(std::getline(in,s)&&s!=std::string("</intrinsic parameter>"));

	}

	void readPoints(){
		in.open(fileName);
		std::string s;
		while(std::getline(in,s)&&s!=std::string("<Feature Tracks>"));

		in>>s;
		int n;
		int kpts=0;
		if(s[0]=='k'){
			in>>kpts;
			in>>n;
		}
		else{
			kpts=0;
			n=std::stoi(s);
		}






		//cout<<n<<"n"<<endl;
		osg::Vec3Array * points=new osg::Vec3Array;
		int m;
		double x,y,z,t;
		for(int i=0;i<n;i++){
			in>>m;
			//cout<<m<<"m"<<endl;
			in>>t>>t;
			in>>x>>y>>z;
			for(int j=0;j<m;j++)
				in>>t>>t>>t;
			for(int j=0;j<kpts;j++)
				in>>t;
			points->push_back(osg::Vec3(x,y,z));
			//	cout<<osg::Vec3(x,y,z)<<endl;

		}

		in.close();


	}

	void readFrame(int seq){

		std::string s;
		//char  t[]="<Image Sequence>";

		while(in>>s&&s.substr(0,6)!=("<FRAME"));
		cout<<s<<endl;
		osg::Matrixd m;
		double f;
		in>>f;
		for(int i=0;i<3;i++){
			for(int j=0;j<4;j++){
				in>>m(i,j);
			}
		}
		m(3,0)=m(3,1)=m(3,2)=0;
		m(3,3)=1;
		cout<<m;
		m= transpose(osg::Matrixd::inverse(m));

		while(in>>s&&s.substr(0,6)!=("<FRAME"));
		
		group->addChild(getRec(scale,scale*xp/focal,scale*yp/focal,ip.get(seq),m));

	}




void passFrame(){
	string s;
	while(in>>s&&s.substr(0,6)!=("<FRAME"));
	while(in>>s&&s.substr(0,6)!=("<FRAME"));

}


void readCamera(int seq){
	in.open(fileName);
	string s;
	while(std::getline(in,s)&&s!=std::string("<Camera Track>"));
	cout<<s<<endl;
	int t=0;

	while(in){
		
		if(t==seq)
			readFrame(start+step*t);
		else
			passFrame();

		t++;

	}

	in.close();
}


};

#endif