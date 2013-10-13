
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
#include <osgDB/WriteFile>
#include<fstream>
#include<stdio.h>
#include<iostream>
#include <sstream>
using namespace std;
osg::ref_ptr<osg::Node> getRec(double f, double xp, double yp,const std::string& img,const osg::Matrixd & transform );
int findLast(const std::string & path){
	 int last=-1;
	for(int i=path.size()-1;i>=0;i--){
		if(path[i]=='.'){
			last=i;
			break;
		}
	}
	return last;
}

std::string  get(const std::string & path, int last,int num){
	std::string img=path;
   

	std::stringstream ss;
	ss<<num;
	std::string ns=ss.str();
	for(int i=ns.size()-1;i>=0;i--)
		img[--last]=ns[i];
//	cout<<path<<endl;
	return img;
}

void readFrame(std::istream &in,osg::Group * group,const string & img){
	std::string s;
	//char  t[]="<Image Sequence>";
	
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
	
		//osg::MatrixTransform * matrix=new osg::MatrixTransform;
		
		//matrix->setMatrix(m.inverse(m));
		//matrix->addChild(node);
		//group->addChild(matrix);
	//std::cout<<m<<std::endl;
	group->addChild(getRec(1,1,1,img,m));




}
void read(std::istream & in,osg::Group * group)
{
	std::string s;
	while(std::getline(in,s)&&s!=std::string("<Image Sequence>"));
	std::string imgPath,temp;
	
	int start,step,end;
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
//	cout<<end<<endl;
	int last=findLast(imgPath);
	while(std::getline(in,s)&&s!=std::string("<Camera Track>"));

	int seq=start;

	while(in){
		 temp=get(imgPath,last,seq);
//		 cout<<temp<<endl;
		 readFrame(in,group,get(imgPath,last,seq));
		
		 seq+=step;
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

osg::ref_ptr<osg::Node> getRec(double f, double xp, double yp,const std::string& img,const osg::Matrixd & transform ){
	osg::Vec3Array* rec=new osg::Vec3Array,*tri;
	rec->push_back(osg::Vec3(-xp,-yp,f)*transform);
	rec->push_back(osg::Vec3(-xp,yp,f)*transform);
	rec->push_back(osg::Vec3(xp,yp,f)*transform);
	rec->push_back(osg::Vec3(xp,-yp,f)*transform);
	rec->push_back(osg::Vec3(-xp,-yp,f)*transform);
	osg::Geometry *g=new osg::Geometry;
	g->setVertexArray(rec);
	g->addPrimitiveSet(new osg::DrawArrays(GL_QUADS,0,4));
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
     normals->push_back( osg::Vec3(0.0f,0.0f, -1.0f)*transform );
	 g->setNormalArray( normals.get() );
     g->setNormalBinding( osg::Geometry::BIND_OVERALL );
	osg::Geometry * tr[4];
	
	osg::Geode *node=new osg::Geode;
	node->addDrawable(g);
	osg::PolygonMode *pm = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
	for(int i=0;i<4;i++){
		tr[i]=new osg::Geometry;
		tri=new osg::Vec3Array;
		tri->push_back(osg::Vec3(0,0,0)*transform);
		tri->push_back(rec->operator[](i));
		tri->push_back(rec->operator[](i+1));
		tr[i]->setVertexArray(tri);
		tr[i]->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES,0,4));
		auto stateset=tr[i]->getOrCreateStateSet();
		
	   stateset->setAttributeAndModes( pm, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		node->addDrawable(tr[i]);
	}

	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;
texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
 g->setTexCoordArray( 0, texcoords.get() );

 osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
osg::ref_ptr<osg::Image> image =
osgDB::readImageFile( img );
texture->setImage( image.get() );

node->getOrCreateStateSet()->setTextureAttributeAndModes(
0, texture.get() );
	return node;

}
void main(){
	/*std::ifstream in("C:\\Users\\w\\Documents\\indoor.act");
	
	osg::Group *root=new osg::Group;
	double scale=1;
	auto camera=getRec(4,2,1,"C:\\Users\\w\\Pictures\\a.PNG");
	root->addChild(camera.get());*/
	//read(in,root,camera);
	
//	osg::MatrixTransform *tr=new osg::MatrixTransform;
//	tr->addChild(camera);
	
	 
//	root->addChild(tr);
	osg::Group *root=new osg::Group;
	std::ifstream in("C:\\Users\\w\\Documents\\indoor.act");
	read(in,root);
	osgViewer::Viewer viewer;
	
	viewer.setSceneData(root);
//	std::cout<<root->getNumChildren();
//	auto stateset=root->getOrCreateStateSet();
	//osg::PolygonMode *pm = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
//	stateset->setAttributeAndModes( pm, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
//	osgDB::writeNodeFile(*root,"H:\\osgview\\OpenSceneGraph-Data-3.0.0\\camera.osg");
	viewer.run();
//	system("pause");

}