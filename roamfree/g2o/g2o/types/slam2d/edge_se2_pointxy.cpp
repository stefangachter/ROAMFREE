// g2o - General Graph Optimization
// Copyright (C) 2011 R. Kuemmerle, G. Grisetti, W. Burgard
// 
// g2o is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// g2o is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "edge_se2_pointxy.h"

#ifdef WINDOWS
#include <windows.h>
#endif

#ifdef G2O_HAVE_OPENGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif

namespace g2o {

  EdgeSE2PointXY::EdgeSE2PointXY() :
    BaseBinaryEdge<2, Vector2d, VertexSE2, VertexPointXY>()
  {
  }

  bool EdgeSE2PointXY::read(std::istream& is)
  {
    is >> measurement()[0] >> measurement()[1];
    inverseMeasurement() = measurement() * -1;
    is >> information()(0,0) >> information()(0,1) >> information()(1,1);
    information()(1,0) = information()(0,1);
    return true;
  }

  bool EdgeSE2PointXY::write(std::ostream& os) const
  {
    os << measurement()[0] << " " << measurement()[1] << " ";
    os << information()(0,0) << " " << information()(0,1) << " " << information()(1,1);
    return os.good();
  }

  void EdgeSE2PointXY::initialEstimate(const OptimizableGraph::VertexSet& from, OptimizableGraph::Vertex* to)
  {
    assert(from.size() == 1 && from.count(_vertices[0]) == 1 && "Can not initialize VertexSE2 position by VertexPointXY");

    VertexSE2* vi     = static_cast<VertexSE2*>(_vertices[0]);
    VertexPointXY* vj = static_cast<VertexPointXY*>(_vertices[1]);
    if (from.count(vi) > 0 && to == vj) {
      vj->estimate() = vi->estimate() * _measurement;
    }
  }

#ifndef NUMERIC_JACOBIAN_TWO_D_TYPES
  void EdgeSE2PointXY::linearizeOplus()
  {
    const VertexSE2* vi     = static_cast<const VertexSE2*>(_vertices[0]);
    const VertexPointXY* vj = static_cast<const VertexPointXY*>(_vertices[1]);
    const double& x1        = vi->estimate().translation()[0];
    const double& y1        = vi->estimate().translation()[1];
    const double& th1       = vi->estimate().rotation().angle();
    const double& x2        = vj->estimate()[0];
    const double& y2        = vj->estimate()[1];

    double aux_1 = cos(th1) ;
    double aux_2 = -aux_1 ;
    double aux_3 = sin(th1) ;

    _jacobianOplusXi( 0 , 0 ) = aux_2 ;
    _jacobianOplusXi( 0 , 1 ) = -aux_3 ;
    _jacobianOplusXi( 0 , 2 ) = aux_1*y2-aux_1*y1-aux_3*x2+aux_3*x1 ;
    _jacobianOplusXi( 1 , 0 ) = aux_3 ;
    _jacobianOplusXi( 1 , 1 ) = aux_2 ;
    _jacobianOplusXi( 1 , 2 ) = -aux_3*y2+aux_3*y1-aux_1*x2+aux_1*x1 ;

    _jacobianOplusXj( 0 , 0 ) = aux_1 ;
    _jacobianOplusXj( 0 , 1 ) = aux_3 ;
    _jacobianOplusXj( 1 , 0 ) = -aux_3 ;
    _jacobianOplusXj( 1 , 1 ) = aux_1 ;
  }
#endif

  EdgeSE2PointXYWriteGnuplotAction::EdgeSE2PointXYWriteGnuplotAction(): WriteGnuplotAction(typeid(EdgeSE2PointXY).name()){}

  HyperGraphElementAction* EdgeSE2PointXYWriteGnuplotAction::operator()(HyperGraph::HyperGraphElement* element, HyperGraphElementAction::Parameters* params_){
    if (typeid(*element).name()!=_typeName)
      return 0;
    WriteGnuplotAction::Parameters* params=static_cast<WriteGnuplotAction::Parameters*>(params_);
    if (!params->os){
      std::cerr << __PRETTY_FUNCTION__ << ": warning, on valid os specified" << std::endl;
      return 0;
    }

    EdgeSE2PointXY* e =  static_cast<EdgeSE2PointXY*>(element);
    VertexSE2* fromEdge = static_cast<VertexSE2*>(e->vertices()[0]);
    VertexPointXY* toEdge   = static_cast<VertexPointXY*>(e->vertices()[1]);
    *(params->os) << fromEdge->estimate().translation().x() << " " << fromEdge->estimate().translation().y()
      << " " << fromEdge->estimate().rotation().angle() << std::endl;
    *(params->os) << toEdge->estimate().x() << " " << toEdge->estimate().y() << std::endl;
    *(params->os) << std::endl;
    return this;
  }

#ifdef G2O_HAVE_OPENGL
  EdgeSE2PointXYDrawAction::EdgeSE2PointXYDrawAction(): DrawAction(typeid(EdgeSE2PointXY).name()){}

  HyperGraphElementAction* EdgeSE2PointXYDrawAction::operator()(HyperGraph::HyperGraphElement* element, 
								HyperGraphElementAction::Parameters* /* params_ */){
    if (typeid(*element).name()!=_typeName)
      return 0;
    EdgeSE2PointXY* e =  static_cast<EdgeSE2PointXY*>(element);
    VertexSE2* fromEdge = static_cast<VertexSE2*>(e->vertices()[0]);
    VertexPointXY* toEdge   = static_cast<VertexPointXY*>(e->vertices()[1]);
    glColor3f(0.4,0.4,0.2);
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glVertex3f(fromEdge->estimate().translation().x(),fromEdge->estimate().translation().y(),0.);
    glVertex3f(toEdge->estimate().x(),toEdge->estimate().y(),0.);
    glEnd();
    glPopAttrib();
    return this;
  }
#endif

} // end namespace
