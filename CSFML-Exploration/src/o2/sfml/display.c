#pragma once
#include <o2/sfml/display.h>
#include <assert.h>

void o2_display_pushShape(o2_sfVertexBatch* batch, sfVertex* vertices, size_t count) {
	assert(NULL != batch);
	assert(NULL != vertices);
	assert(batch->count + count <= batch->capacity);

	for (size_t i = 0; i < count; i++) {
		batch->vertices[batch->count + i] = vertices[i];
	}

	batch->count += count;
}

sfVertex* o2_display_getNextVertexDestination(o2_sfVertexBatch* batch, size_t count) {
	assert(NULL != batch);
	assert(batch->count + count <= batch->capacity);

	size_t originalCount = batch->count;
	batch->count += count;

	return batch->vertices + originalCount;
}

void o2_display_positionQuad(sfVertex* vertices, o2_Rect bounds) {
	assert(NULL != vertices);

	vertices[0].position = (sfVector2f){ bounds.x           , bounds.y };
	vertices[1].position = (sfVector2f){ bounds.x + bounds.w, bounds.y };
	vertices[2].position = (sfVector2f){ bounds.x           , bounds.y + bounds.h };
	vertices[3].position = (sfVector2f){ bounds.x + bounds.w, bounds.y + bounds.h };
}

void o2_display_colorQuad(sfVertex* vertices, sfColor color) {
	assert(NULL != vertices);

	vertices[0].color = color;
	vertices[1].color = color;
	vertices[2].color = color;
	vertices[3].color = color;
}

void o2_display_texureQuad(sfVertex* vertices, o2_Rect texCoords) {
	assert(NULL != vertices);

	vertices[0].texCoords = (sfVector2f){ texCoords.x              , texCoords.y };
	vertices[1].texCoords = (sfVector2f){ texCoords.x + texCoords.w, texCoords.y };
	vertices[2].texCoords = (sfVector2f){ texCoords.x              , texCoords.y + texCoords.h };
	vertices[3].texCoords = (sfVector2f){ texCoords.x + texCoords.w, texCoords.y + texCoords.h };
}

bool o2_display_button(o2_sfUIContext* ctx, o2_sfVertexBatch* batch, uint32 id, o2_Rect bounds, o2_ButtonStyle* style) {
	assert(NULL != ctx);
	assert(NULL != batch);
	assert(batch->primitiveType == sfTriangleStrip);

	sfVertex* vertices = o2_display_getNextVertexDestination(batch, 4);
	o2_display_positionQuad(vertices, bounds);
	o2_display_texureQuad(vertices, style->textureRect);

	if (ctx->activeId == id and ctx->mouseDown) {
		o2_display_colorQuad(vertices, sfBlue);
	}
	else if (ctx->hoverId == id) {
		o2_display_colorQuad(vertices, sfGreen);
	}
	else {
		o2_display_colorQuad(vertices, sfWhite);
	}

	if (o2_geometry_rectContainsPoint(bounds, ctx->mousePos)) {
		ctx->candidateId = id;
		return ctx->mouseClicked and ctx->activeId == id;
	}

	return false;
}
