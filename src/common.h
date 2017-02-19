#define LIBSUPERDERPY_DATA_TYPE struct CommonResources
#include <libsuperderpy.h>

struct CommonResources {
		// Fill in with common data accessible from all gamestates.
		bool touch;
};

struct CommonResources* CreateGameData(struct Game *game);
void DestroyGameData(struct Game *game, struct CommonResources *data);
void DrawCharacterHelper(struct Game *game, struct Character *character, float scalex, float scaley, int flags);
