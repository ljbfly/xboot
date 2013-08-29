/*
 * driver/led/led-simple.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <led/led-simple.h>

struct led_simple_runtime_data_t {
	u32_t color;
	struct led_simple_data_t * rdat;
};

static void led_simple_init(struct led_t * led)
{
	struct led_simple_runtime_data_t * dat = (struct led_simple_runtime_data_t *)led->priv;
	struct led_simple_data_t * rdat = (struct led_simple_data_t *)dat->rdat;

	if(rdat->init)
		rdat->init(rdat);
}

static void led_simple_exit(struct led_t * led)
{
	struct led_simple_runtime_data_t * dat = (struct led_simple_runtime_data_t *)led->priv;
	struct led_simple_data_t * rdat = (struct led_simple_data_t *)dat->rdat;

	dat->color = 0;
	if(rdat->set)
		rdat->set(rdat, dat->color);
}

static void led_simple_set(struct led_t * led, u32_t color)
{
	struct led_simple_runtime_data_t * dat = (struct led_simple_runtime_data_t *)led->priv;
	struct led_simple_data_t * rdat = (struct led_simple_data_t *)dat->rdat;

	dat->color = color;
	if(rdat->set)
		return rdat->set(rdat, dat->color);
}

static u32_t led_simple_get(struct led_t * led)
{
	struct led_simple_runtime_data_t * dat = (struct led_simple_runtime_data_t *)led->priv;

	return dat->color;
}

static void led_simple_suspend(struct led_t * led)
{
}

static void led_simple_resume(struct led_t * led)
{
}

static bool_t led_simple_register_led(struct resource_t * res)
{
	struct led_simple_data_t * rdat = (struct led_simple_data_t *)res->data;
	struct led_simple_runtime_data_t * dat;
	struct led_t * led;
	char name[64];

	dat = malloc(sizeof(struct led_simple_runtime_data_t));
	if(!dat)
		return FALSE;

	led = malloc(sizeof(struct led_t));
	if(!led)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%s", res->name, rdat->name);

	dat->color = 0x0;
	dat->rdat = rdat;

	led->name = strdup(name);
	led->init = led_simple_init;
	led->exit = led_simple_exit;
	led->set = led_simple_set,
	led->get = led_simple_get,
	led->suspend = led_simple_suspend,
	led->resume	= led_simple_resume,
	led->priv = dat;

	if(register_led(led))
		return TRUE;

	free(led->name);
	free(led);
	return FALSE;
}

static bool_t led_simple_unregister_led(struct resource_t * res)
{
	struct led_simple_data_t * dat = (struct led_simple_data_t *)res->data;
	struct led_t * led;
	char name[64];

	snprintf(name, sizeof(name), "%s.%s", res->name, dat->name);

	led = search_led(name);
	if(!led)
		return FALSE;

	if(!unregister_led(led))
		return FALSE;

	free(led->priv);
	free(led->name);
	free(led);
	return TRUE;
}

static __init void led_simple_device_init(void)
{
	resource_callback_with_name("led.simple", led_simple_register_led);
}

static __exit void led_simple_device_exit(void)
{
	resource_callback_with_name("led.simple", led_simple_unregister_led);
}

device_initcall(led_simple_device_init);
device_exitcall(led_simple_device_exit);