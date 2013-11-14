#include "Act.h"
void main(){
	osgViewer::Viewer viewer;
	string fn="C:\\Users\\w\\Documents\\another.act";
	std::string acts[]={"H:\\osgview\\OpenSceneGraph-Data-3.0.0\\acts\\enft.act","C:\\Users\\w\\Documents\\indoors.act","H:\\model\\00856\\test.act"};
	std::string models[]={"H:\\osgview\\OpenSceneGraph-Data-3.0.0\\acts\\_model0.obj","H:\\osgview\\OpenSceneGraph-Data-3.0.0\\acts\\indoor\\indoor\\Model\\PMVS.obj","H:\\model\\00856\\Rec\\Model\\PMVS.obj"};
	osg::Group *root=new osg::Group;
	Act act;
	act.set(acts[0]);
	act.readCamera(30);
	root->addChild(act.group.get());
	cout<<act.group->getNumChildren();
	//osg::Node * model=osgDB::readNodeFile(models[2],new osgDB::Options("noRotation"));
	
	//root->addChild(model);
	viewer.setUpViewInWindow(400, 400, 640, 480); 
	viewer.run();
	//system("pause");

}