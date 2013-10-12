
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Point>
#include <osg/PolygonOffset>
#include <osgUtil/SmoothingVisitor>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include<osgViewer/Viewer>
#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>

#include <osgGA/TerrainManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <osg/Material>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/MatrixTransform>
#include <osg/Camera>
#include <osg/io_utils>
#include <osg/ShapeDrawable>

#include <osgText/Text>

#include <sstream>
#include <cmath>

#include<osgDB/ReadFile>
#include<fstream>
#include<stdio.h>
#include<iostream>


void readFrame(std::istream &in,osg::Group * group,osg::Node * node){
	std::string s;
	while(in>>s&&s.substr(0,6)!=("<FRAME"));
	osg::Matrixd m;
	double f;
	in>>f;
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			in>>m(j,i);
		}
	}
	
	while(in>>s&&s.substr(0,6)!=("<FRAME"));
	
		osg::MatrixTransform * matrix=new osg::MatrixTransform;
		
		matrix->setMatrix(m.inverse(m));
		matrix->addChild(node);
		group->addChild(matrix);
	//std::cout<<m<<std::endl;
}
void read(std::istream & in,osg::Group * group,osg::Node * node)
{
	std::string s;
	while(std::getline(in,s)&&s!=std::string("<Camera Track>"));
	while(in)
		 readFrame(in,group,node);

}

void read_act(const char * path,osg::Group * group,osg::Node * node)
{

	FILE* file=fopen(path,"rt");
	if(file==NULL)
	{
		fprintf(stderr,"Open File: %s Error!",path);
		exit(1);
	}
	const int MAXN = 100;
	int start,step,end,num_frames=0;
	std::string imagePath;
	char *str=new char[MAXN+1];

	while(fgets(str,MAXN,file)&&strcmp(str,"<Image Sequence>\n"));
	fgets(str,MAXN,file);//
	imagePath=str;
	fscanf(file,"start:%d\n",&start);
	fscanf(file,"step:%d\n",&step);
	fscanf(file,"end:%d\n",&end);
	
	while(fgets(str,MAXN,file)&&strcmp(str,"<intrinsic parameter>\n"));
	double fx,fy,skew,x0,y0,aspect_ratio;
	fscanf(file,"%lf %lf %lf %lf %lf %lf\n",&fx,&fy,&x0,&y0,&skew,&aspect_ratio);

	while(fgets(str,MAXN,file)&&strcmp(str,"<Camera Track>\n"));

	std::cout<<std::string(str);
	int num=0;
	/*while(fgets(str,MAXN,file)){
		
		std::cout<<num++<<std::string(str);
	}*/
	for(int i=start;i<=end;i+=step){
		fgets(str,MAXN,file);
		std::cout<<std::string(str);
		fgets(str,MAXN,file);
		//std::cout<<str;
		double d[4][4];
		for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				fscanf(file,"%d",&d[i][j]);
				std::cout<<d[i][j]<<" ";
			}
			//std::cout<<std::endl;
		}
	//	if(num%10==0){
	//	osg::MatrixTransform * matrix=new osg::MatrixTransform;
	//	matrix->setMatrix(d);
	//	matrix->addChild(node);
	//	group->addChild(matrix);
	////	std::cout<<d;
	//	}
	//	num++;
		fgets(str,MAXN,file);
		fgets(str,MAXN,file);
	//	std::cout<<str;
	}

}


















osg::ref_ptr<osg::Node> getRec(double f, double xp, double yp){
	osg::Vec3Array* rec=new osg::Vec3Array,*tri;
	rec->push_back(osg::Vec3(-xp,-yp,f));
	rec->push_back(osg::Vec3(-xp,yp,f));
	rec->push_back(osg::Vec3(xp,yp,f));
	rec->push_back(osg::Vec3(xp,-yp,f));
	rec->push_back(osg::Vec3(-xp,-yp,f));
	osg::Geometry *g=new osg::Geometry;
	g->setVertexArray(rec);
	g->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,4));

	osg::Geometry * tr[4];
	osg::Geode *node=new osg::Geode;
	node->addDrawable(g);
	for(int i=0;i<4;i++){
		tr[i]=new osg::Geometry;
		tri=new osg::Vec3Array;
		tri->push_back(osg::Vec3(0,0,0));
		tri->push_back(rec->operator[](i));
		tri->push_back(rec->operator[](i+1));
		tr[i]->setVertexArray(tri);
		tr[i]->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES,0,4));
		node->addDrawable(tr[i]);
	}
	return node;

}

void main(){
	std::ifstream in("C:\\Users\\w\\Documents\\indoor.act");
	
	osg::Group *root=new osg::Group;
	double scale=1;
	auto camera=getRec(4,2,1);
	read(in,root,camera);
	
//	osg::MatrixTransform *tr=new osg::MatrixTransform;
//	tr->addChild(camera);
	
	 
//	root->addChild(tr);

	osgViewer::Viewer viewer;
	
	viewer.setSceneData(root);
	std::cout<<root->getNumChildren();
	auto stateset=root->getOrCreateStateSet();
	osg::PolygonMode *pm = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
	stateset->setAttributeAndModes( pm, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	viewer.run();
//	system("pause");

}