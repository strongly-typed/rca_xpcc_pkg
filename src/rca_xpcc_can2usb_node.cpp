#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rca_xpcc_pkg/Can.h"

// XPCC
#include <xpcc/architecture/interface/can.hpp>
#include <xpcc/architecture/platform/driver/can/hosted/canusb.hpp>

static constexpr uint32_t canBusBaudRate = 125000;

xpcc::hosted::CanUsb canUsb;

/**
 * Simple ROS node that uses xpcc CAN2USB parser.
 *
 * ToDo:
 * - Parameters:
 *   - Baud Rate
 *   - Port
 * - Send messages to CAN bus
 */
int
main(int argc, char **argv)
{
    ros::init(argc, argv, "talker");
    ros::NodeHandle n;
    ros::Publisher can2usb_pub = n.advertise<rca_xpcc_pkg::Can>("can2usb", 1000);
    ros::Rate loop_rate(10);

    ROS_DEBUG("Hello CAN2USB");
    
	if (!canUsb.open("/dev/ttyUSB1", canBusBaudRate)) {
    	ROS_ERROR("Could not open port");
    }

    while (ros::ok())
    {
        // Process messages from CAN bus
		while (canUsb.isMessageAvailable())
		{
			xpcc::can::Message message;
			canUsb.getMessage(message);
			
            rca_xpcc_pkg::Can msg;
            msg.header.stamp = ros::Time::now();
            msg.identifier   = message.getIdentifier();
            msg.rtr          = message.isRemoteTransmitRequest();
            msg.extended     = message.isExtended();
            
            for (std::size_t ii = 0; ii < message.getLength(); ++ii) {
                msg.data.push_back(message.data[ii]);
            }
            
            can2usb_pub.publish(msg);
		}

        ros::spinOnce();

        loop_rate.sleep();
    }
    
    return 0;
}

