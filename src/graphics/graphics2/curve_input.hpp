/* Created by Darren Otgaar on 2016/07/09. http://www.github.com/otgaard/zap */
#ifndef ZAP_CURVE_INPUT_HPP
#define ZAP_CURVE_INPUT_HPP

/* The curve_input class will capture the mouse and allow specification of a curve.  Initially designed for the hermite
 * curve, the curve_input will be abstracted to allow input of Bezier curves and Catmull-Rom curves as well.
 */

class curve_input {
public:
    curve_input();
    ~curve_input();
};


#endif //ZAP_CURVE_INPUT_HPP
