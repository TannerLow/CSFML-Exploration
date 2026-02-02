#include <stdio.h>
#include <CSFML/Graphics.h>
#include <CSFML/Graphics/Color.h>
#include <CSFML/System.h>
#include <string.h>
#include <o2/types.h>
#include <o2/arena.h>
#include <o2/math.h>

void printError(const char* message, const char* file, int lineNumber);
void simpleNumToString(char* output, int num, int maxDigits);

int main() {
	int returnCode = EXIT_SUCCESS;
	sfRenderWindow* window = NULL;
	sfRectangleShape* rectangle = NULL;
	sfFont* arialFont = NULL;
	sfText* fpsText = NULL;
	sfClock* fpsClock = NULL;
	sfClock* fpsLimiterClock = NULL;

	size_t globalArenaSize = 1 * MEGABYTE;
	Arena globalArena = {
		.base = (uint8*)malloc(globalArenaSize),
		.size = globalArenaSize,
		.used = 0
	};

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

		arialFont = sfFont_createFromFile("resources/arial.ttf");
		if (NULL == arialFont) {
			printError("Failed to load arial.ttf", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}

		fpsText = sfText_create(arialFont);
		if (NULL == fpsText) {
			printError("Failed to create FPS text object", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}
		sfText_setPosition(fpsText, (sfVector2f) { 10, 10 });

		char fpsDigits[8];
		const int maxFpsDigits = 7;
		memset(fpsDigits, '\0', 8);
		simpleNumToString(fpsDigits, 0, maxFpsDigits);
		sfText_setString(fpsText, fpsDigits);

		rectangle = sfRectangleShape_create();
		if (NULL == rectangle) {
			printError("Failed to create RectangleShape", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}
		sfRectangleShape_setFillColor(rectangle, sfRed);
		sfRectangleShape_setPosition(rectangle, (sfVector2f) { 400, 400 });
		sfRectangleShape_setSize(rectangle, (sfVector2f) { 200, 100 });

		fpsClock = sfClock_create();
		if (NULL == fpsClock) {
			printError("Failed to create FPS clock", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}
		sfTime frameTime;

		fpsLimiterClock = sfClock_create();
		if (NULL == fpsLimiterClock) {
			printError("Failed to create FPS limiter clock", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}
		sfTime frameLimiterTime;

		MovingAverage_int32 fpsMovingAverage;
		//o2_arena_pushAligned(&globalArena, sizeof(MovingAverage_int32), alignof(int32));
		fpsMovingAverage.capacity = 60;
		fpsMovingAverage.values = o2_arena_pushArrayAligned(
			&globalArena, 
			sizeof(int32), 
			fpsMovingAverage.capacity, 
			alignof(int32)
		);
		o2_math_movingAverageInit(&fpsMovingAverage, fpsMovingAverage.values, fpsMovingAverage.capacity);

		sfClock_restart(fpsClock);
		sfClock_restart(fpsLimiterClock);

		sfEvent event;
		while (sfRenderWindow_isOpen(window)) {

			while (sfRenderWindow_pollEvent(window, &event)) {

				if (event.type == sfEvtClosed) {
					sfRenderWindow_close(window);
				}

			}

			if (sfTime_asSeconds(sfClock_getElapsedTime(fpsLimiterClock)) > 1.f / 240.f) {

				frameTime = sfClock_restart(fpsClock);
				int fps = (int)(1.0f / sfTime_asSeconds(frameTime));
				fps = o2_math_movingAverageAdd(&fpsMovingAverage, fps);
				if (fpsMovingAverage.index == 0) {
					simpleNumToString(fpsDigits, fps, maxFpsDigits);
					sfText_setString(fpsText, fpsDigits);
				}

				sfRenderWindow_clear(window, sfBlack);
				sfRenderWindow_drawRectangleShape(window, rectangle, NULL);
				sfRenderWindow_drawText(window, fpsText, NULL);
				sfRenderWindow_display(window);

				sfClock_restart(fpsLimiterClock);
			}
		}
	}

CLEAN_RESOURCES:
	free(globalArena.base);
	sfClock_destroy(fpsLimiterClock);
	sfClock_destroy(fpsClock);
	sfText_destroy(fpsText);
	sfFont_destroy(arialFont);
	sfRectangleShape_destroy(rectangle);
	sfRenderWindow_destroy(window);

	return 0;
}

void printError(const char* message, const char* file, int lineNumber) {
	fprintf(stderr, "%s: (%s, %d)\n", message, file, lineNumber);
}

void simpleNumToString(char* output, int num, int maxDigits) {
	int digitCount = 0;
	
	while (num > 0 && digitCount < maxDigits) {
		output[digitCount] = (num % 10) + '0';
		num /= 10;
		digitCount++;
	}
	
	if (digitCount == 0) {
		output[0] = '0';
		digitCount++;
	}

	output[digitCount] = '\0';

	// reverse digits since they were created from least to most significant
	for (int i = 0; i < digitCount / 2; i++) {
		char temp = output[i];
		output[i] = output[digitCount - 1 - i];
		output[digitCount - 1 - i] = temp;
	}
}