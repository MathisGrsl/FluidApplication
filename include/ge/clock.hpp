namespace ge {
    struct clock {
        int fps = 60;
        double deltaLimit = 1.0 / 60.0;
        double delta = 0.0;

        clock() = default;
        clock(int frame) : fps(frame)
        {
            deltaLimit = 1.0 / double(frame);
        }
        bool on(double deltaTime)
        {
            bool res = false;

            delta += deltaTime;
            if (delta > deltaLimit) {
                res = true;
                delta -= deltaLimit;
            }
            return res;
        }
    };
}
