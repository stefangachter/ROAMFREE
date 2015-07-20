/*
 Copyright (c) 2013-2016 Politecnico di Milano.
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the GNU Lesser Public License v3
 which accompanies this distribution, and is available at
 https://www.gnu.org/licenses/lgpl.html

 Contributors:
 Davide A. Cucci (davide.cucci@epfl.ch)
 */

/*
 * BaseRandomWalkProcessEdge.h
 *
 *  Created on: May 7, 2015
 *      Author: davide
 *
 *      constraint the difference between two vertices
 *      second - first = measurement + noise
 *
 */

#ifndef BASERANDOMWALKPROCESSEDGE_H_
#define BASERANDOMWALKPROCESSEDGE_H_

#include <iomanip>

#include <Eigen/Dense>
#include "ROAMmath/MatricesOperations.h"

#include "g2o/core/base_binary_edge.h"
#include "BasePriorEdgeInterface.h"

namespace ROAMestimation {

template<int D, typename VertexXi>

class BaseRandomWalkProcessEdge: public g2o::BaseBinaryEdge<D, Eigen::VectorXd,
    VertexXi, VertexXi>, public BasePriorEdgeInterface {

  protected:

    using g2o::BaseBinaryEdge<D, Eigen::VectorXd, VertexXi, VertexXi>::_measurement;
    using g2o::BaseBinaryEdge<D, Eigen::VectorXd, VertexXi, VertexXi>::_information;

    Eigen::Matrix<double, D, D> _noiseCov;

  public:

    virtual ~BaseRandomWalkProcessEdge() {
    }

    virtual void setNoiseCov(const Eigen::MatrixXd &noiseCov) {
      _noiseCov = noiseCov;

      // TODO: this causes an extra inversion for those edges which have non-identity jacobian wrt error
      ROAMmath::inv(_noiseCov, _information);
    }

    virtual void setMeasurement(const Eigen::VectorXd &measurement) {
      _measurement = measurement;
    }

    virtual g2o::OptimizableGraph::Edge *getg2oOptGraphPointer() {
      return static_cast<g2o::OptimizableGraph::Edge *>(this);
    }

    bool read(std::istream &s) {
      return false;
    }

    bool write(std::ostream &s) const {
      return false;
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

} /* namespace ROAMestimation */
#endif /* BASERANDOMWALKPROCESSEDGE_H_ */