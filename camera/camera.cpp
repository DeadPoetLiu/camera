
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

const double scale=3;
osg::Vec3Array* readPoints(istream & in){
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




	
	
	cout<<n<<"n"<<endl;
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

	return points;
}




osg::Vec3Array* cp=new osg::Vec3Array;
double focal,xp,yp;
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
	cout<<img<<endl;
	return img;
}

osg::Matrixd transpose(const osg::Matrixd & m){
	osg::Matrixd re;
	for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				re(i,j)=m(j,i);
			}
		}
	return re;

}

void readFrame(std::istream &in,osg::Group * group,const string & img){
	static int se=0;
	std::string s;
	//char  t[]="<Image Sequence>";
	
	while(in>>s&&s.substr(0,6)!=("<FRAME"));
	cout<<s<<endl;
	osg::Matrixd m;
	double f;
	in>>f;
	cout<<f<<endl;
		for(int i=0;i<3;i++){
			for(int j=0;j<4;j++){
				in>>m(i,j);
				cout<<m(i,j)<<" ";
			}
			cout<<endl;
		}

		m(3,0)=m(3,1)=m(3,2)=0;
		m(3,3)=1;
		m= transpose(osg::Matrixd::inverse(m));
	/*for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				in>>m(i,j);
			}
		}

	 m(3,0)=m(0,3);
	 m(0,3)=0;
	 m(3,1)=m(1,3);
	 m(1,3)=0;
	m(3,2)=m(2,3);
	m(2,3)=0;*/
	
	
	while(in>>s&&s.substr(0,6)!=("<FRAME"));
	if(!in)
		cout<<"finish";
		//osg::MatrixTransform * matrix=new osg::MatrixTransform;
		
		//matrix->setMatrix(m.inverse(m));
		//matrix->addChild(node);
		//group->addChild(matrix);
	//std::cout<<m<<std::endl;
	//getRec(1,1,1,img,m);
	
	  // group->addChild(getRec(100,100.0*xp/focal,100.0*yp/focal,img,m));
	   if(se%10==1){
		   group->addChild(getRec(scale,scale*xp/focal,scale*yp/focal,img,m));
	       cout<<"one camera"<<endl;
	   }

	   
	    se++;

}

void addPoints(std::istream & in,osg::Group * group){
	osg::Vec3Array* points=readPoints(in);

	osg::Geometry * ps=new osg::Geometry();
	ps->setVertexArray(points);

	ps->addPrimitiveSet(new osg::DrawArrays(GL_POINTS,0,points->size()));
	osg::Geode * pps=new osg::Geode;
	pps->addDrawable(ps);
	group->addChild(pps);

	//osgDB::writeNodeFile(*pps,"points.obj");

	//group->addChild(osgDB::readNodeFile("points.obj"));

}


void read(std::istream & in,osg::Group * group,bool readPoints)
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

	while(std::getline(in,s)&&s!=std::string("<intrinsic parameter>"));
	double a,b,c,d,e,f;
	in>>a>>b>>c>>d;

	focal=a;
	xp=c;
	yp=d;

	if(readPoints)
		addPoints(in, group);
	
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
	static int seq=0;
	osg::Vec3Array* rec=new osg::Vec3Array,*tri;
	rec->push_back(osg::Vec3(xp,yp,f)*transform);
	rec->push_back(osg::Vec3(-xp,yp,f)*transform);
	rec->push_back(osg::Vec3(-xp,-yp,f)*transform);
	rec->push_back(osg::Vec3(xp,-yp,f)*transform);
	rec->push_back(osg::Vec3(xp,-yp,f)*transform);
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
	auto p=osg::Vec3(0,0,0)*transform;
	cp->push_back(p);

	for(int i=0;i<4;i++){
		tr[i]=new osg::Geometry;
		tri=new osg::Vec3Array;
		tri->push_back(p);
		tri->push_back(rec->operator[](i));
		tri->push_back(rec->operator[](i+1));
		tr[i]->setVertexArray(tri);
		tr[i]->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES,0,4));
		auto stateset=tr[i]->getOrCreateStateSet();
		
	   stateset->setAttributeAndModes( pm, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		node->addDrawable(tr[i]);
	}

	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

		texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
texcoords->push_back( osg::Vec2(1.0f, 1.0f) );

	/*if(seq==0){
texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
	}
	else if(seq==1){
		
texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
 texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
	}else if(seq==2){
		texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
texcoords->push_back( osg::Vec2(1.0f, 1.0f) );


	}else{
		texcoords->push_back( osg::Vec2(0.0f, 0.0f) );
		texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
	}*/
	seq++;
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
	string fn="C:\\Users\\w\\Documents\\another.act";
	std::string acts[]={"H:\\osgview\\OpenSceneGraph-Data-3.0.0\\acts\\enft.act","C:\\Users\\w\\Documents\\indoors.act","H:\\model\\00856\\test.act"};
	std::string models[]={"H:\\osgview\\OpenSceneGraph-Data-3.0.0\\acts\\_model0.obj","H:\\osgview\\OpenSceneGraph-Data-3.0.0\\acts\\indoor\\indoor\\Model\\PMVS.obj","H:\\model\\00856\\Rec\\Model\\PMVS.obj"};
	osg::Group *root=new osg::Group;
	std::ifstream in(acts[2]);
	if(in)
	   read(in,root,false);
	else
		cout<<"error";
	osg::Geometry *ps=new osg::Geometry;
	ps->setVertexArray(cp);
	ps->addPrimitiveSet( new osg::DrawArrays(GL_LINE_STRIP,0,cp->size()));
	osg::Geode *gj=new osg::Geode;
	gj->addDrawable(ps);
	ps->getOrCreateStateSet()->setAttribute( new osg::Point( 100.0f ), osg::StateAttribute::ON ); 
	root->addChild(gj);
	osgViewer::Viewer viewer;
	
	viewer.setSceneData(root);
	std::cout<<root->getNumChildren();
//	auto stateset=root->getOrCreateStateSet();
	//osg::PolygonMode *pm = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
//	stateset->setAttributeAndModes( pm, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
//	osgDB::writeNodeFile(*root,"H:\\osgview\\OpenSceneGraph-Data-3.0.0\\camera.osg");
	//root->addChild( osgDB::readNodeFile(models[1]) );
///	osgDB::ReaderWriter::Options.setOptionString("
	osg::Node * model=osgDB::readNodeFile(models[2],new osgDB::Options("noRotation"));
	
	root->addChild(model);
	viewer.setUpViewInWindow(400, 400, 640, 480); 
	viewer.run();
	

}