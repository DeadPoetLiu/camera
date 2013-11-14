#include "headers.h"

inline osg::Matrixd transpose(const osg::Matrixd & m){
	osg::Matrixd re;
	for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				re(i,j)=m(j,i);
			}
		}
	return re;
}

inline osg::ref_ptr<osg::Node> getRec(double xp,double yp,double focal,const std::string& img,const osg::Matrixd & transform ){

	osg::Vec3Array* rec=new osg::Vec3Array,*tri;
	rec->push_back(osg::Vec3(xp,yp,focal)*transform);
	rec->push_back(osg::Vec3(-xp,yp,focal)*transform);
	rec->push_back(osg::Vec3(-xp,-yp,focal)*transform);
	rec->push_back(osg::Vec3(xp,-yp,focal)*transform);
	rec->push_back(osg::Vec3(xp,-yp,focal)*transform);
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


	texcoords->push_back( osg::Vec2(0.0f, 1.0f) );
	texcoords->push_back( osg::Vec2(1.0f, 1.0f) );
	texcoords->push_back( osg::Vec2(1.0f, 0.0f) );
	texcoords->push_back( osg::Vec2(0.0f, 0.0f) );

	g->setTexCoordArray( 0, texcoords.get() );

	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	osg::ref_ptr<osg::Image> image =
		osgDB::readImageFile( img );
	texture->setImage( image.get() );

	node->getOrCreateStateSet()->setTextureAttributeAndModes(
		0, texture.get() );
	return node;

}