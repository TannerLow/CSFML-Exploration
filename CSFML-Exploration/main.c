#include <stdio.h>
#include <CSFML/Graphics.h>
#include <CSFML/Graphics/Color.h>
#include <CSFML/System.h>
#include <string.h>
#include <o2/types.h>
#include <o2/arena.h>
#include <o2/math.h>
#include <o2/sfml/display.h>
#include <o2/geometry.h>
#include <o2/display.h>

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
	sfTexture* texture = NULL;
	sfTexture* uiSpriteSheet = NULL;

	size_t globalArenaSize = 1 * MEGABYTE;
	o2_Arena globalArena = {
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

		texture = sfTexture_createFromFile("resources/attack on ct.png", NULL);
		if (NULL == texture) {
			printError("Failed to load image: resources/attack on ct.png", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}
		sfVector2u textureSize = sfTexture_getSize(texture);

		uiSpriteSheet = sfTexture_createFromFile("resources/squid3.png", NULL);
		if (NULL == uiSpriteSheet) {
			printError("Failed to load image: resources/squid3.png", __FILE__, __LINE__);
			returnCode = EXIT_FAILURE;
			goto CLEAN_RESOURCES;
		}
		sfVector2u uiSpriteSheetSize = sfTexture_getSize(uiSpriteSheet);

		const size_t maxVertexInBatch = 100;
		o2_sfVertexBatch batch = {
			.vertices = o2_arena_pushArrayAligned(&globalArena, sizeof(sfVertex), maxVertexInBatch, alignof(float32)),
			.primitiveType = sfTriangleStrip,
			.renderStates = sfRenderStates_default,
			.capacity = maxVertexInBatch,
			.count = 0
		};
		batch.renderStates.texture = texture;

		const size_t maxVertexInUIBatch = 100;
		o2_sfVertexBatch uiBatch = {
			.vertices = o2_arena_pushArrayAligned(&globalArena, sizeof(sfVertex), maxVertexInUIBatch, alignof(float32)),
			.primitiveType = sfTriangleStrip,
			.renderStates = sfRenderStates_default,
			.capacity = maxVertexInUIBatch,
			.count = 0
		};
		uiBatch.renderStates.texture = uiSpriteSheet;

		sfVertex* vertices = o2_display_getNextVertexDestination(&batch, 4);
		vertices[0].position = (sfVector2f){ 100, 100 };
		vertices[1].position = (sfVector2f){ 500, 100 };
		vertices[2].position = (sfVector2f){ 100, 300 };
		vertices[3].position = (sfVector2f){ 500, 300 };
		vertices[0].color = sfWhite;
		vertices[1].color = sfWhite;
		vertices[2].color = sfWhite;
		vertices[3].color = sfWhite;
		vertices[0].texCoords = (sfVector2f){ 0, 0 };
		vertices[1].texCoords = (sfVector2f){ (float)textureSize.x, 0 };
		vertices[2].texCoords = (sfVector2f){ 0, (float)textureSize.y };
		vertices[3].texCoords = (sfVector2f){ (float)textureSize.x, (float)textureSize.y };

		o2_sfUIContext uiContext = {0};

		o2_Rect buttonRect = {
			.x = 700,
			.y = 300,
			.w = 200,
			.h = 100
		};
		o2_Rect buttonTexRect = {
			.x = 0,
			.y = 0,
			.w = 768,
			.h = 768
		};
		o2_ButtonStyle buttonStyle = {
			.textureRect = buttonTexRect,
			.hoverTextureRect = buttonTexRect,
			.activeTextureRect = buttonTexRect
		};

		printf("Global arena usage pre-game loop: %llu\n", globalArena.used);

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

		int fpsCap = 240;
		o2_MovingAverage_int32 fpsMovingAverage;
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

				if (event.type == sfEvtMouseMoved) {
					sfVector2f mousePos = sfRenderWindow_mapPixelToCoords(
						window, 
						event.mouseMove.position, 
						sfRenderWindow_getView(window)
					);
					uiContext.mousePos.x = mousePos.x;
					uiContext.mousePos.y = mousePos.y;
				}
				else if (event.type == sfEvtMouseButtonPressed) {
					uiContext.mouseDown = true;
					uiContext.activeId = uiContext.candidateId;
				}
				else if (event.type == sfEvtMouseButtonReleased) {
					uiContext.mouseDown = false;
					uiContext.mouseClicked = true;
				}
				else if (event.type == sfEvtClosed) {
					sfRenderWindow_close(window);
				}
			}

			uiContext.hoverId = uiContext.candidateId;

			if (sfTime_asSeconds(sfClock_getElapsedTime(fpsLimiterClock)) > 1.f / fpsCap) {

				// average fps display update
				frameTime = sfClock_restart(fpsClock);
				int fps = (int)(1.0f / sfTime_asSeconds(frameTime));
				fps = o2_math_movingAverageAdd(&fpsMovingAverage, fps);
				if (fpsMovingAverage.index == 0) {
					simpleNumToString(fpsDigits, fps, maxFpsDigits);
					sfText_setString(fpsText, fpsDigits);
				}

				uiBatch.count = 0;
				uiContext.candidateId = 0;
				if (o2_display_button(&uiContext, &uiBatch, __LINE__, buttonRect, &buttonStyle)) {
					printf("Button clicked\n");
				}
				if (uiContext.mouseClicked) {
					uiContext.mouseClicked = false;
					uiContext.activeId = 0;
				}

				sfRenderWindow_clear(window, sfBlack);
				sfRenderWindow_drawRectangleShape(window, rectangle, NULL);
				sfRenderWindow_drawText(window, fpsText, NULL);
				sfRenderWindow_drawPrimitives(
					window, 
					batch.vertices, 
					batch.count, 
					batch.primitiveType, 
					&batch.renderStates
				);
				sfRenderWindow_drawPrimitives(
					window, 
					uiBatch.vertices, 
					uiBatch.count, 
					uiBatch.primitiveType, 
					&uiBatch.renderStates
				);
				sfRenderWindow_display(window);

				sfClock_restart(fpsLimiterClock);
			}
		}
	}

CLEAN_RESOURCES:
	free(globalArena.base);
	sfTexture_destroy(uiSpriteSheet);
	sfTexture_destroy(texture);
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