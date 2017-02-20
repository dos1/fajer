/*! \file empty.c
 *  \brief Empty gamestate.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "../common.h"
#include <libsuperderpy.h>

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_FONT *font;
		int blink_counter;

		ALLEGRO_SAMPLE *sample;
		ALLEGRO_SAMPLE_INSTANCE *broom;

		ALLEGRO_BITMAP *exit1, *exit2, *exit3, *bg;

		float pos;
};

int Gamestate_ProgressCount = 5; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->blink_counter++;

	if (data->blink_counter == 200) {
		al_play_sample_instance(data->broom);
	}

	if (data->blink_counter > 310) {
		data->pos-=0.01;
	}

	if (data->blink_counter > 420) {
		SwitchCurrentGamestate(game, "win");
	}
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_draw_scaled_bitmap(data->bg, 0, 0, al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg), 0, 0, game->viewport.width, game->viewport.height, 0);

	ALLEGRO_BITMAP *exit = data->exit1;


	if (data->blink_counter > 120) {
		exit = data->exit2;
	}

	if (data->blink_counter > 140) {
		exit = data->exit1;
	}
	if (data->blink_counter > 180) {
		exit = data->exit2;
	}


	if (data->blink_counter > 260) {
		exit = data->exit3;
	}

	al_draw_scaled_bitmap(exit, 0, 0, al_get_bitmap_width(exit), al_get_bitmap_height(exit), game->viewport.width * data->pos, 0, game->viewport.width, game->viewport.height, 0);

}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->font = al_create_builtin_font();
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->bg= al_load_bitmap(GetDataFilePath(game, "exitbg.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	data->exit1= al_load_bitmap(GetDataFilePath(game, "exit1.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	data->exit2= al_load_bitmap(GetDataFilePath(game, "exit2.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	data->exit3= al_load_bitmap(GetDataFilePath(game, "exit3.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->sample = al_load_sample(GetDataFilePath(game, "brum.flac"));
	data->broom = al_create_sample_instance(data->sample);
	al_attach_sample_instance_to_mixer(data->broom, game->audio.fx);

	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	al_destroy_sample_instance(data->broom);
	al_destroy_sample(data->sample);
	al_destroy_bitmap(data->bg);
	al_destroy_bitmap(data->exit1);
	al_destroy_bitmap(data->exit2);
	al_destroy_bitmap(data->exit3);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->blink_counter = 0;
	data->pos = 0;
}

void Gamestate_Stop(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
}

void Gamestate_Pause(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic not ProcessEvent)
	// Pause your timers here.
}

void Gamestate_Resume(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers here.
}

// Ignore this for now.
// TODO: Check, comment, refine and/or remove:
void Gamestate_Reload(struct Game *game, struct GamestateResources* data) {}
