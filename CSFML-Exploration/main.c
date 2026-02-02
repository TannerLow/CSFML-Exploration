#include <stdio.h>
#include <CSFML/Graphics.h>
#include <CSFML/Graphics/Color.h>

void printError(const char* message, const char* file, int lineNumber);

int main() {
	int returnCode = EXIT_SUCCESS;
	sfRenderWindow* window = NULL;
	sfRectangleShape* rectangle = NULL;

	{
		sfVector2u videoModeSize;
		videoModeSize.x = 1280;
		videoModeSize.y = 720;

		sfVideoMode videoMode;
		videoMode.size = videoModeSize;
		videoMode.bitsPerPixel = 32;

		window = sfRenderWindow_create(videoMode, "SFML Window", sfDefaultStyle, sfWindowed, NULL);
		if (NULL == window) {
			printError("Failed to create RenderWindow", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}

		rectangle = sfRectangleShape_create();
		if (NULL == window) {
			printError("Failed to create RectangleShape", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}
		sfRectangleShape_setFillColor(rectangle, sfRed);
		sfRectangleShape_setPosition(rectangle, (sfVector2f) { 400, 400 });
		sfRectangleShape_setSize(rectangle, (sfVector2f) { 200, 100 });

		sfEvent event;
		while (sfRenderWindow_isOpen(window)) {

			while (sfRenderWindow_pollEvent(window, &event)) {

				if (event.type == sfEvtClosed) {
					sfRenderWindow_close(window);
				}

			}

			sfRenderWindow_clear(window, sfBlack);
			sfRenderWindow_drawRectangleShape(window, rectangle, NULL);
			sfRenderWindow_display(window);
		}
	}

	CLEAN_RESOURCES:
	sfRectangleShape_destroy(rectangle);
	sfRenderWindow_destroy(window);

	return 0;
}

void printError(const char* message, const char* file, int lineNumber) {
	fprintf(stderr, "%s: (%s, %d)", message, file, lineNumber);
}