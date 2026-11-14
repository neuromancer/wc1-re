/* Positional sound effects pan towards the side of the view where their
 * object is drawn (issue #24). The DOS AdLib, FM Towns and Kilrathi Saga
 * effect players all take 0 as left and 127 as right. No game assets are
 * needed. */
#include "wc1.h"

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        exit(1);
    }
}

static void SetCamera(FixedVector right, FixedVector up, FixedVector forward)
{
    aShipRightVector[EYE_OBJECT] = right;
    aShipUpVector[EYE_OBJECT] = up;
    aShipFwdVec[EYE_OBJECT] = forward;
}

/* The pan for a unit direction, after checking that the view draws that
 * direction on the same side: transform_objects_to_your_view puts positive
 * view x right of the centre. */
static int Pan(int x, int y, int z)
{
    FixedVector direction = {x, y, z};
    FixedVector view;
    int pan = SdlSoundEffectPan(&direction);

    transform_to_objects_frame(&direction, &view, EYE_OBJECT);
    Require((view.x > 0) == (pan > 64) && (view.x < 0) == (pan < 64),
            "the pan follows the side the view draws the object on");
    return pan;
}

int main(void)
{
    SetCamera((FixedVector){0x100, 0, 0}, (FixedVector){0, 0x100, 0},
              (FixedVector){0, 0, 0x100});
    Require(Pan(0x100, 0, 0) == 127 && Pan(-0x100, 0, 0) == 0,
            "an object to the right is heard on the right, and to the left on the left");
    /* A quarter of 45 degrees' offset each way, rounded down. */
    Require(Pan(0xb5, 0, 0xb5) == 109 && Pan(-0xb5, 0, 0xb5) == 18,
            "ahead and to a side pans part way");
    Require(Pan(0, 0, 0x100) == 64 && Pan(0, 0x100, 0) == 64 && Pan(0, 0, -0x100) == 64,
            "ahead, above and behind are centred");

    /* Turned a quarter: the camera's right is now world -z. */
    SetCamera((FixedVector){0, 0, -0x100}, (FixedVector){0, 0x100, 0},
              (FixedVector){0x100, 0, 0});
    Require(Pan(0, 0, -0x100) == 127 && Pan(0, 0, 0x100) == 0 && Pan(0x100, 0, 0) == 64,
            "turning the camera moves the sound with the view");
    puts("PASS: positional effects pan to the side of the view their object is on.");
    return 0;
}
