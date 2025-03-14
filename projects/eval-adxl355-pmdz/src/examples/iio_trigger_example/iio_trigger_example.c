/***************************************************************************//**
 *   @file   iio_trigger_example.c
 *   @brief  Implementation of IIO trigger example for eval-adxl355-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "iio_trigger_example.h"
#include "iio_adxl355.h"
#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define DATA_BUFFER_SIZE 400

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
uint8_t iio_data_buffer[DATA_BUFFER_SIZE*3*sizeof(int)];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO trigger example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run_with_trigs and will
 * 				 not return.
*******************************************************************************/
int iio_trigger_example_main ()
{
	int ret;

	struct adxl355_iio_trig *adxl355_iio_trig;
	struct adxl355_iio_trig *adxl355_iio_sw_trig;
	struct adxl355_iio_dev *adxl355_iio_desc;
	struct adxl355_iio_dev_init_param adxl355_iio_init_par;
	struct no_os_irq_ctrl_desc *adxl355_irq_ctrl;
	struct iio_desc *iio_desc;

	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE*3*sizeof(int)
	};

	adxl355_iio_init_par.adxl355_dev_init = &adxl355_user_init;
	ret = adxl355_iio_init(&adxl355_iio_desc, &adxl355_iio_init_par);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_init(&adxl355_irq_ctrl,
				  adxl355_iio_trig_user_init.irq_init_param);
	if (ret)
		return ret;
	adxl355_iio_trig_user_init.irq_ctrl = adxl355_irq_ctrl;

	/* Initialize hardware trigger */
	adxl355_iio_trig_user_init.iio_desc = &iio_desc,
	iio_adxl355_trigger_init(&adxl355_iio_trig, &adxl355_iio_trig_user_init);

	/* Initialize software trigger */
	adxl355_iio_sw_trig_user_init.iio_desc = &iio_desc;
	iio_adxl355_software_trigger_init(&adxl355_iio_sw_trig,
					  &adxl355_iio_sw_trig_user_init);

	/* List of devices */
	struct iio_app_device iio_devices[] = {
		{
			.name = "adxl355",
			.dev = adxl355_iio_desc,
			.dev_descriptor = adxl355_iio_desc->iio_dev,
			.read_buff = &accel_buff,
		}
	};

	/* List of triggers */
	struct iio_trigger_init trigs[] = {
		IIO_APP_TRIGGER(IIO_ADXL355_TRIGGER_NAME, adxl355_iio_trig,
				&adxl355_iio_trigger_desc),
		IIO_APP_TRIGGER(IIO_ADXL355_SW_TRIGGER_NAME, adxl355_iio_sw_trig,
				&adxl355_iio_software_trigger_desc)
	};

	return iio_app_run_with_trigs(iio_devices, NO_OS_ARRAY_SIZE(iio_devices),
				      trigs, NO_OS_ARRAY_SIZE(trigs), adxl355_irq_ctrl, &iio_desc);
}
