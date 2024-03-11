#pragma once

template<class VEC>
class axBezierCurveHelper {
public:
	using T = typename VEC::Element;
	static VEC getPoint(const VEC& p0, const VEC& p1, const VEC& p2, const VEC& p3, T weight);

	static void evaluateCurve(	axIArray<VEC>& outPoints, 
						const VEC& p0, const VEC& p1,const VEC& p2,const VEC& p3,
						T maxSegmentLength = ax_inf_<T>(), T flatness = T(1.001), axInt maxRecursiveDepth = 16)
	{
		outPoints.clear();
		EvalRequest req;
		req.outPoints = &outPoints;
		req.maxSegmentLength = maxSegmentLength;
		req.flatness = flatness;
		req.maxRecursiveDepth = maxRecursiveDepth;

		req.outPoints->append(p0);
		_recursiveEval(req, p0, p1, p2, p3, 0);
	}

private:
	struct EvalRequest : public axNonCopyable {
		axIArray<VEC>* outPoints = nullptr;
		T maxSegmentLength = ax_inf_<T>();
		T flatness = T(1.001);
		axInt maxRecursiveDepth = 12;
	};

	static void _recursiveEval(EvalRequest& req, const VEC& p0, const VEC& p1,const VEC& p2,const VEC& p3, axInt depth);
};

//----------------
template<class VEC> inline
VEC axBezierCurveHelper<VEC>::getPoint(const VEC& p0, const VEC& p1, const VEC& p2, const VEC& p3, T weight)
{
	T w = weight;
	T i = 1 - w;
	T ww = w * w;
	T ii = i * i;

	return p0 * (ii * i)
		+ p1 * (3 * ii * w)
		+ p2 * (3 * i * ww)
		+ p3 * (ww * w);
}

template<class VEC> inline
void axBezierCurveHelper<VEC>::_recursiveEval(EvalRequest& req, const VEC& p0, const VEC& p1,const VEC& p2,const VEC& p3, axInt depth) {
	//  p1----------p12----------p2
	//   |          / \          |
	//   |         /   \         |
	//   |        /     \        |
	//   |       /       \       |
	//   |      /         \      |
	//   |  p012---p0123---p123  |
	//   |    /             \    |
	//   |   /               \   |
	//   |  /                 \  |
	//   | /                   \ |
	//  p12                     p23
	//   |                       |
	//   |                       |
	//   |                       |
	//   |                       |
	//   |                       |
	//  p0                       p3
	//

	auto m = p0.distance(p1) + p1.distance(p2) + p2.distance(p3);
	auto d = p0.distance(p3);
	if (m < req.flatness * d && d < req.maxSegmentLength) {
		req.outPoints->append(p3);
		return;
	}

	depth++;
	if (depth >= req.maxRecursiveDepth)
		return;

	auto p01 = (p0 + p1) * T(0.5);
	auto p12 = (p1 + p2) * T(0.5);
	auto p23 = (p2 + p3) * T(0.5);
	auto p012 = (p01 + p12) * T(0.5);
	auto p123 = (p12 + p23) * T(0.5);
	auto p0123 = (p012 + p123) * T(0.5);

	_recursiveEval(req, p0, p01, p012, p0123, depth);
	_recursiveEval(req, p0123, p123, p23, p3, depth);
}



