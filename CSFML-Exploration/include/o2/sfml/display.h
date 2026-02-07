#pragma once
#include <CSFML/Graphics.h>
#include <o2/geometry.h>
#include <stdbool.h>
#include <o2/display.h>

typedef struct o2_sfVertexBatch {
	sfVertex* vertices;
	sfPrimitiveType primitiveType;
	sfRenderStates renderStates;
	size_t capacity;
	size_t count;
} o2_sfVertexBatch;

typedef struct o2_sfUIContext {
	uint32 activeId;
	uint32 hoverId;
	uint32 candidateId;
	o2_Point mousePos;
	bool mouseDown;
	bool mouseClicked;
} o2_sfUIContext;

void o2_display_pushShape(o2_sfVertexBatch* batch, sfVertex* vertices, size_t count);
// Consume count number of vertices in batch and receive a pointer for in-place modification
sfVertex* o2_display_getNextVertexDestination(o2_sfVertexBatch* batch, size_t count);

void o2_display_positionQuad(sfVertex* vertices, o2_Rect bounds);
void o2_display_colorQuad(sfVertex* vertices, sfColor color);
void o2_display_texureQuad(sfVertex* vertices, o2_Rect texCoords);

bool o2_display_button(o2_sfUIContext* ctx, o2_sfVertexBatch* batch, uint32 id, o2_Rect bounds, o2_ButtonStyle* style);
