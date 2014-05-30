
#ifndef _CAMERA_H_
#define _CAMERA_H_

class Camera
{
private:
    float x;
    float y;
    int width;
    int height;
    int mapWidth;
    int mapHeight;

public:
    Camera(float x_, float y_, int width_, int height_, int mapWidth_, int mapHeight_)
    {
        x = x_;
        y = y_;
        width = width_;
        height = height_;
        mapWidth = mapWidth_;
        mapHeight = mapHeight_;
    }
    ~Camera()
    {

    }

    void update()
    {
        if(x < 0)
            x = 0;
        if(x + width > mapWidth)
            x = mapWidth - width;

        if(y < 0)
            y = 0;
        if(y + height > mapHeight)
            y = mapHeight - height;
    }

    void moveH(float deltaX)
    {
        x += deltaX;
    }

    void moveV(float deltaY)
    {
        y += deltaY;
    }

    float getX() {return x;}
    float getY() {return y;}
    int getWidth() {return width;}
    int getHeight() {return height;}

};

#endif
