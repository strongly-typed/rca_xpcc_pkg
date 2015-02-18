// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------
/**
 * @ingroup		processing
 * @defgroup	coroutine		Coroutines
 *
 * An implementation of lightweight coroutines which allow for nested calling.
 *
 * This base class and its macros allows you to implement and use several
 * coroutines in one class.
 * This allows you to modularize your code by placing it into its own coroutines
 * instead of the placing everything into one big method.
 * It also allows you to call and run coroutines within your coroutines,
 * so you can reuse their functionality.
 *
 * Note that you should call coroutines within a protothreads.
 * It is sufficient to use the `this` pointer of the class as context
 * when calling the coroutines.
 * So calling a coroutine is done using `PT_CALL(coroutine(this))`
 * which will return the result of the coroutine.
 *
 * You may use the `CO_CALL_BLOCKING(coroutine(ctx))` macro to execute
 * a coroutine outside of a protothread, however, this which will
 * force the CPU to busy-wait until the coroutine ended.
 *
 * Here is a (slightly over-engineered) example:
 *
 * @code
 * #include <xpcc/architecture.hpp>
 * #include <xpcc/processing/protothread.hpp>
 * #include <xpcc/coroutine/coroutine.hpp>
 * #include <xpcc/processing/timer.hpp>
 *
 * typedef GpioOutputB0 Led;
 *
 * class BlinkingLight : public xpcc::pt::Protothread, private xpcc::co::NestedCoroutine<2>
 * {
 * public:
 *     bool
 *     run()
 *     {
 *         PT_BEGIN();
 *
 *         // set everything up
 *         Led::setOutput();
 *         Led::set();
 *
 *         while (true)
 *         {
 *             Led::set();
 *             PT_CALL(waitForTimer(this)))
 *
 *             Led::reset();
 *             PT_CALL(setTimer(this, 200));
 *
 *             PT_WAIT_UNTIL(timeout.isExpired());
 *         }
 *
 *         PT_END();
 *     }
 *
 *     xpcc::co::Result<bool>
 *     waitForTimer(void *ctx)
 *     {
 *         CO_BEGIN();
 *
 *         // nested calling is allowed
 *         if (CO_CALL(setTimer(ctx, 100)))
 *         {
 *             CO_WAIT_UNTIL(timeout.isExpired());
 *         }
 *
 *         CO_END_RETURN(false);
 *     }
 *
 *     xpcc::co::Result<bool>
 *     setTimer(void *ctx, uint16_t timeout)
 *     {
 *         CO_BEGIN();
 *
 *         timeout.restart(timeout);
 *
 *         if(timeout.isRunning())
 *             CO_RETURN(true);
 *
 *         // clean up code goes here
 *
 *         CO_END_RETURN(false);
 *     }
 *
 * private:
 *     xpcc::ShortTimeout timeout;
 * };
 *
 *
 * ...
 * BlinkingLight light;
 *
 * while (...) {
 *     light.run();
 * }
 * @endcode
 *
 * For other examples take a look in the `examples` folder in the XPCC
 * root folder.
 */

#include "coroutine/coroutine.hpp"
#include "coroutine/nested_coroutine.hpp"
