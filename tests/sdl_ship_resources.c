#include "wc1.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    const enum ObjectType fighterTypes[3] = {
        OBJECT_TYPE_SCIMITAR,
        OBJECT_TYPE_SALTHI,
        OBJECT_TYPE_HEAT_SEEKING_MISSILE
    };
    ObjectResourceSlot *resource;
    ObjectTypeData *typeData;
    short slot;
    short section;

    /* Returning to the carrier frees slots 1 and 2 even when the last nav
     * transition has already unloaded them and set their types to -1. */
    memset(aObjectResourceSlots, 0, sizeof(aObjectResourceSlots));
    for (slot = 0; slot < 4; slot++) {
        resource = &aObjectResourceSlots[slot];
        resource->type = -1;
        if (free_ship(slot) != 0 || resource->type != -1 ||
            resource->shapeSet != 0 || resource->animation != 0 ||
            resource->shape != 0) {
            fprintf(stderr, "Empty resource slot %d was not preserved\n", slot);
            return 1;
        }
    }

    /* Ordinary fighters and missiles release all three packets and clear
     * the shared type and object pointers. Releasing twice is harmless. */
    for (slot = 0; slot < 3; slot++) {
        resource = &aObjectResourceSlots[slot];
        resource->type = (signed char)fighterTypes[slot];
        typeData = &aObjectTypeData[fighterTypes[slot]];
        resource->shapeSet = AllocateTaggedMemory(8, 0);
        resource->animation = AllocateTaggedMemory(8, 0);
        resource->shape = AllocateTaggedMemory(8, 0);
        if (resource->shapeSet == 0 || resource->animation == 0 ||
            resource->shape == 0)
            return 1;
        typeData->shapeSet = resource->shapeSet;
        typeData->animation = resource->animation;
        typeData->shape = resource->shape;
        aeObjectType[slot] = fighterTypes[slot];
        aeObjectClass[slot] = typeData->objectClass;
        apObjectShape[slot] = resource->shapeSet;
        if (free_ship(slot) != 0 || free_ship(slot) != 0 ||
            resource->type != fighterTypes[slot] ||
            resource->shapeSet != 0 || resource->animation != 0 ||
            resource->shape != 0 || typeData->shapeSet != 0 ||
            typeData->animation != 0 || typeData->shape != 0 ||
            apObjectShape[slot] != 0) {
            fprintf(stderr, "Loaded resource slot %d was not released\n", slot);
            return 1;
        }
    }

    /* Capital ships have no shapeSet, but still own a shape, view frames,
     * and (with expanded memory) 37 cached archive sections. */
    slot = 3;
    resource = &aObjectResourceSlots[slot];
    resource->type = OBJECT_TYPE_TIGERS_CLAW;
    typeData = &aObjectTypeData[OBJECT_TYPE_TIGERS_CLAW];
    resource->shape = AllocateTaggedMemory(8, 0);
    typeData->shape = resource->shape;
    aeObjectClass[1] = OBJECT_CLASS_CAPITAL_SHIP;
    aeObjectType[1] = OBJECT_TYPE_TIGERS_CLAW;
    apObjectShape[1] = AllocateTaggedMemory(8, 0);
    asCapitalShipViewFrame[1] = 3;
    if (resource->shape == 0 || apObjectShape[1] == 0)
        return 1;
    bExpandedMemoryAvailable = 1;
    for (section = 0; section < 37; section++) {
        aapPacketReferences[slot][section] = AllocateTaggedMemory(8, 4);
        if (aapPacketReferences[slot][section] == 0)
            return 1;
    }
    if (free_ship(slot) != 0 || free_ship(slot) != 0 ||
        resource->shape != 0 || typeData->shape != 0 ||
        apObjectShape[1] != 0 || asCapitalShipViewFrame[1] != -1) {
        fprintf(stderr, "Capital ship resources were not released\n");
        return 1;
    }
    for (section = 0; section < 37; section++) {
        if (aapPacketReferences[slot][section] != 0)
            return 1;
    }
    return 0;
}
