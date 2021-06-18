#include "stun/exp_curve_fitter.h"
#include <boost/ut.hpp>

using namespace boost::ut;

suite stun_exp_curve_fitter = [] {
    "ExpCurveFitter type traits"_test = [] {
        using angonoka::stun::ExpCurveFitter;
        expect(std::is_nothrow_destructible_v<ExpCurveFitter>);
        expect(
            std::is_nothrow_default_constructible_v<ExpCurveFitter>);
        expect(

            std::is_nothrow_copy_constructible_v<ExpCurveFitter>);
        expect(std::is_nothrow_copy_assignable_v<ExpCurveFitter>);
        expect(std::is_nothrow_move_constructible_v<ExpCurveFitter>);
        expect(std::is_nothrow_move_assignable_v<ExpCurveFitter>);
    };

    "ExpCurveFitter basic operations"_test = [] {
        using angonoka::stun::ExpCurveFitter;
        ExpCurveFitter fit;

        expect(fit(0.F, 0.F) == 0._d);
        fit.reset();
        fit(1.F, .011108996538242F);
        fit(2.F, .018315638888734F);
        fit(3.F, .030197383422319F);
        fit(4.F, .049787068367864F);
        expect(fit(5.F, .082084998623899F) == .0820852_d);
        fit(6.F, .135335283236613F);
        fit(7.F, .22313016014843F);
        fit(8.F, .367879441171442F);
        fit(9.F, .606530659712634F);
        expect(fit(10.F, 1.F) == 1.0_d);

        expect(fit.at(10.F) == 1.0_d);
        expect(fit.at(0.F) == 0.0_d);
        expect(fit.at(1.F) == 0.011109_d);

        fit.reset();
        expect(fit(0.F, 0.F) == 0._d);
    };
};
