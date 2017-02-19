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
#include <allegro5/allegro_color.h>
#include <math.h>

#define HUMANS 3

struct Mov {
		float dx;
		float dy;
		bool dead;
		int type;
};

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_FONT *font;
		float cloudpos;
		ALLEGRO_BITMAP *clouds;

		struct Character *ego;
		struct Character *humans[HUMANS];
		struct Character *building, *tramp, *van;

		ALLEGRO_BITMAP *bg;

		ALLEGRO_SAMPLE *sample;
		ALLEGRO_SAMPLE_INSTANCE *music;


		ALLEGRO_SAMPLE *boomsample;
		ALLEGRO_SAMPLE_INSTANCE *boom;
		ALLEGRO_SAMPLE *bdziumsample;
		ALLEGRO_SAMPLE_INSTANCE *bdzium;
		ALLEGRO_SAMPLE *boingsample;
		ALLEGRO_SAMPLE_INSTANCE *boing;


		unsigned long long ticks;

		float fire;

		bool left, right;

		int score, progress;

};

float POINTS[3] = {0.4375, 0.5625, 0.71875};
char* TYPES[3] = {"kid", "woman", "man"};

int Gamestate_ProgressCount = 9; // number of loading steps as reported by Gamestate_Load

void SetTarget(struct Game *game, struct Character *character, int point, float ypos) {
	float target = POINTS[point];
	struct Mov *mov = character->data;
	mov->dx = (character->x - target) / 60;
//	mov->dy
}

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	for (int i=0; i<HUMANS; i++) {
	AnimateCharacter(game, data->humans[i], 1);
	}
	AnimateCharacter(game, data->building, 1);
	AnimateCharacter(game, data->ego, 1);
	AnimateCharacter(game, data->tramp, 1);
	AnimateCharacter(game, data->van, 1);

	data->progress++;

	data->cloudpos -= 0.02;
	if (data->cloudpos < -1) {
		data->cloudpos = 1;
	}
data->ticks++;

if (data->progress > 60*60) {
	SwitchCurrentGamestate(game, "lose");
}


for (int i=0; i<HUMANS; i++) {

struct Mov *mov = data->humans[i]->data;
mov->dy += 0.25;
mov->dx += 0.01;

MoveCharacter(game, data->humans[i], mov->dx, mov->dy, -0.03);
}

int move = 0;
if (data->left) move--;
if (data->right) move++;

  MoveCharacterF(game, data->ego, move * 0.005, 0, 0);

	if (data->ego->x < 0.28) data->ego->x = 0.28;
	if (data->ego->x > 0.7) data->ego->x = 0.7;

if (data->ticks % 6 == 0) {
	data->fire = rand() / (float)INT_MAX * 0.142;
}

for (int i=0; i<HUMANS; i++) {
	struct Mov *mov = data->humans[i]->data;


if (data->humans[i]->x < -0.3) {
	if (!mov->dead) {
		SelectSpritesheet(game, data->humans[i], "boom");
		mov->dy = 0;
		al_stop_sample_instance(data->boom);
		al_play_sample_instance(data->boom);
		mov->dead = true;
	}
}

if ((data->humans[i]->y < 0.725) && (data->humans[i]->y > 0.675))  {

if ((data->humans[i]->x + 0.2 < data->tramp->x + 0.3) && (data->humans[i]->x - 0.2 > data->tramp->x - 0.25)) {
//if (IsOnCharacter(game, data->tramp, data->human->x * game->viewport.width, (data->human->y + 0.1) * game->viewport.height)) {
 if(!mov->dead) {
	mov->dy = -20;
	if (mov->type==0) {
//		mov->dx = -7;
		mov->dx = (data->humans[i]->x - 0.23)* -24;

	} else {
		mov->dx = -20;
	}
	al_stop_sample_instance(data->boing);
	al_play_sample_instance(data->boing);
 }
}
 else if (IsOnCharacter(game, data->van, data->humans[i]->x * game->viewport.width, (data->humans[i]->y) * game->viewport.height)) {
 if (!mov->dead) {
	SetCharacterPositionF(game, data->humans[i], 0.8, 0.27, 0);
	mov->dx = (rand() / (float)INT_MAX) * -12;
	mov->dy = -10;
	al_stop_sample_instance(data->bdzium);
	al_play_sample_instance(data->bdzium);
	mov->type = rand() % 3;
	data->score++;
	if (data->score > 10) {
		SwitchCurrentGamestate(game, "exit");
	}
	SelectSpritesheet(game, data->humans[i], TYPES[mov->type]);
 }
} else {
	if (!mov->dead) {
		SelectSpritesheet(game, data->humans[i], "boom");
		mov->dy = 0;
		al_stop_sample_instance(data->boom);
		al_play_sample_instance(data->boom);
		mov->dead = true;
	}
}
}

if (mov->dead) {
	if (!data->humans[i]->successor) {
		SetCharacterPositionF(game, data->humans[i], 0.8, 0.27, 0);
		mov->dx = (rand() / (float)INT_MAX) * -12;
		mov->dy = -10;
		mov->dead = false;
		mov->type = rand() % 3;
		SelectSpritesheet(game, data->humans[i], TYPES[mov->type]);
	}
}


}
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_clear_to_color(al_map_rgb(30,198,228));


//	PrintConsole(game, "%f %f", );
	al_draw_scaled_bitmap(data->clouds, 0, 0, al_get_bitmap_width(data->clouds), al_get_bitmap_height(data->clouds), (data->cloudpos+1) * game->viewport.width, 0,game->viewport.width, game->viewport.height, 0);
	al_draw_scaled_bitmap(data->clouds, 0, 0, al_get_bitmap_width(data->clouds), al_get_bitmap_height(data->clouds),data->cloudpos * game->viewport.width, 0, game->viewport.width, game->viewport.height,0);
	al_draw_scaled_bitmap(data->clouds, 0, 0, al_get_bitmap_width(data->clouds), al_get_bitmap_height(data->clouds), (data->cloudpos-1) * game->viewport.width, 0, game->viewport.width, game->viewport.height,0);

	al_draw_scaled_bitmap(data->bg, 0, 0, al_get_bitmap_width(data->bg), al_get_bitmap_height(data->bg), 0, 0, game->viewport.width, game->viewport.height, 0);


	al_draw_filled_rectangle(game->viewport.width * 0.8, game->viewport.height * 0.2, game->viewport.width, game->viewport.height * 0.9,
	                         al_color_hsv(0, 1, 0.5 - data->fire));

	DrawCharacterHelper(game, data->building, 1, 1, 0);



	SetCharacterPosition(game, data->tramp, GetCharacterX(game, data->ego) + 0.0666 * game->viewport.width,  0.725 * game->viewport.height, 0);


	DrawCharacterHelper(game, data->ego, 0.5, 0.5, 0);
	DrawCharacterHelper(game, data->tramp, 0.3573, 0.3573, 0);
	for (int i=0; i<HUMANS; i++) {
	DrawCharacterHelper(game, data->humans[i], 1, 1, 0);
	}
	DrawCharacterHelper(game, data->van, 1, 1, 0);


	al_draw_filled_rounded_rectangle(game->viewport.width * 0.2, game->viewport.height * 0.01, game->viewport.width * 0.8, game->viewport.height * 0.07, game->viewport.width / 320, game->viewport.height / 180, al_map_rgb(0,0,0));
	al_draw_filled_rectangle(game->viewport.width * 0.21, game->viewport.height * 0.02,
	                         game->viewport.width * 0.21 + (game->viewport.width * 0.58) * (data->progress / (60.0*60.0)),
	                         game->viewport.height * 0.06, al_map_rgb(255,0,0));


	char buf[255];
	snprintf(buf, 255, "%d", data->score);

	DrawTextWithShadow(data->font, al_map_rgb(255, 255, 255), game->viewport.width * 0.141, game->viewport.height * 0.716, ALLEGRO_ALIGN_CENTER,
	                   buf);
}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		//UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
		SwitchCurrentGamestate(game, "menu");
	}
bool movement = data->left || data->right;
  if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
		if (ev->keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
			SwitchCurrentGamestate(game, "lose");
		}
		  if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT) {
			data->left = true;
			if (!movement)
				SelectSpritesheet(game, data->ego, "walk");
		}
		if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
			data->right = true;
			if (!movement)
				SelectSpritesheet(game, data->ego, "walk");
		}
	}
	if (ev->type == ALLEGRO_EVENT_KEY_UP) {
		if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT) {
			data->left = false;
		}
		if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
			data->right = false;
		}
	}
	if (!(data->left || data->right)) {
		if (movement)
			SelectSpritesheet(game, data->ego, "stand");
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->font = al_load_ttf_font(GetDataFilePath(game, "fonts/DejaVuSansMono.ttf"),
	                              game->viewport.height*0.05 ,0 );

	for (int i=0; i<HUMANS; i++) {

	data->humans[i] = CreateCharacter(game, "human");
	RegisterSpritesheet(game, data->humans[i], "kid");
	RegisterSpritesheet(game, data->humans[i], "man");
	RegisterSpritesheet(game, data->humans[i], "woman");
	RegisterSpritesheet(game, data->humans[i], "boom");
	RegisterSpritesheet(game, data->humans[i], "blank");
	LoadSpritesheets(game, data->humans[i]);
	}
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->building = CreateCharacter(game, "building");
	RegisterSpritesheet(game, data->building, "fire");
	LoadSpritesheets(game, data->building);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->sample = al_load_sample(GetDataFilePath(game, "music.flac"));
	data->music = al_create_sample_instance(data->sample);
	al_attach_sample_instance_to_mixer(data->music, game->audio.music);
	al_set_sample_instance_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(data->music, 0.85);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->bdziumsample = al_load_sample(GetDataFilePath(game, "bdzium.flac"));
	data->bdzium = al_create_sample_instance(data->bdziumsample);
	al_attach_sample_instance_to_mixer(data->bdzium, game->audio.fx);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->boomsample = al_load_sample(GetDataFilePath(game, "boom.flac"));
	data->boom = al_create_sample_instance(data->boomsample);
	al_attach_sample_instance_to_mixer(data->boom, game->audio.fx);
	al_set_sample_instance_gain(data->boom, 1.5);

	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->boingsample = al_load_sample(GetDataFilePath(game, "boing.flac"));
	data->boing = al_create_sample_instance(data->boingsample);
	al_attach_sample_instance_to_mixer(data->boing, game->audio.fx);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar


	data->ego = CreateCharacter(game, "ego");
	RegisterSpritesheet(game, data->ego, "walk");
	RegisterSpritesheet(game, data->ego, "stand");
	LoadSpritesheets(game, data->ego);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->van = CreateCharacter(game, "van");
	RegisterSpritesheet(game, data->van, "van");
	LoadSpritesheets(game, data->van);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar


	data->tramp = CreateCharacter(game, "tramp");
	RegisterSpritesheet(game, data->tramp, "tramp");
	LoadSpritesheets(game, data->tramp);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->bg = al_load_bitmap(GetDataFilePath(game, "bg.png"));
	data->clouds = al_load_bitmap(GetDataFilePath(game, "clouds.png"));


	for (int i=0; i<HUMANS; i++) {
	struct Mov *mov = malloc(sizeof(struct Mov));
	mov->dx = -4 * i;
	mov->dy = -5 * i;
	mov->dead = false;
	mov->type = 0;
	data->humans[i]->data = mov;
}
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	al_destroy_bitmap(data->bg);
	DestroyCharacter(game, data->building);
//	DestroyCharacter(game, data->human);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->score=0;
	data->progress = 0;
	data->cloudpos = 1;
	data->ticks = 0;
	data->fire = 0;
	data->left = false;
	data->right = false;
	SetCharacterPositionF(game, data->ego, 0.2875, 0.666, 0);
	SelectSpritesheet(game, data->ego, "stand");
for (int i=0; i<HUMANS; i++) {
	SelectSpritesheet(game, data->humans[i], "kid");
	SetCharacterPositionF(game, data->humans[i], 0.8, 0.27, 0);
}
  SelectSpritesheet(game, data->building, "fire");
	SetCharacterPositionF(game, data->building, 0.76, 0.1, 0);
	SelectSpritesheet(game, data->tramp, "tramp");
	SelectSpritesheet(game, data->van, "van");
	SetCharacterPositionF(game, data->van, 0, 0.6, 0);
	al_play_sample_instance(data->music);
}

void Gamestate_Stop(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
	al_stop_sample_instance(data->music);
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
