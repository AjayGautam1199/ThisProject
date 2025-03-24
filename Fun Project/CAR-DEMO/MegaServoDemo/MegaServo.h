#define MAX_SERVOS 2

#define MIN_POT_VALUE 180
#define MAX_POT_VALUE 800
 
#define MIN_ANGLE 10
#define MAX_ANGLE 170

typedef unsigned char uint8_t;

typedef struct {
	int wanted_angle;
	int current_angle;
	uint8_t speed;
} servo_status;

class MegaServo
{
public:
	MegaServo();
	void attach(int _cw_pin, int _ccw_pin, int _enablerPin, uint8_t _sensor_pin);
        void attach(int _cw_pin, int _ccw_pin, int _enablerPin, uint8_t _sensor_pin, int _offset);
	void detach();
	void write(int degrees);
	void update();
	servo_status read();

        bool active;
        int index;
private:                
        int offset;
	int cw_pin;
	int ccw_pin;
	int enabler_pin;
	uint8_t sensor_pin;
	uint8_t speed;
	int wanted_position;
	int tolerance;
	
        void write_pot_value(int degrees);
        void run_cw();
	void run_ccw();
	void stop();
        int get_position();
};

static int s_index;                // number of registered servo's
static MegaServo* _m_servos[2];    // array containing registered servo's

