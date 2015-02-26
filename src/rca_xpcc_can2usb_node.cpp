#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rca_xpcc_pkg/Can.h"

// XPCC
#include <xpcc/architecture/interface/can.hpp>
#include <xpcc/architecture/platform/driver/can/hosted/canusb.hpp>

xpcc::hosted::CanUsb canUsb;

void
can2usbCallback(const rca_xpcc_pkg::Can::ConstPtr& msg)
{
	ROS_DEBUG("I got %d with %d bytes", msg->identifier, msg->data.size());
	
	// Build xpcc CAN message
	xpcc::can::Message message;
	message.setIdentifier(msg->identifier);
	message.setRemoteTransmitRequest(msg->rtr);
	message.setExtended(msg->extended);
	
	for (std::size_t ii = 0; ii < msg->data.size(); ++ii) {
		message.data[ii] = msg->data[ii];
	}
	message.setLength(msg->data.size());
	
	ROS_DEBUG("XPCC Can message is:");
	ROS_DEBUG("  Identifier = %d", message.getIdentifier());
	ROS_DEBUG("  RTR        = %d", message.isRemoteTransmitRequest());
	ROS_DEBUG("  extended   = %d", message.isExtended());
	ROS_DEBUG("  Length     = %d", message.getLength());
	ROS_DEBUG("  DATA       = %02x %02x %02x %02x %02x %02x %02x %02x",
		message.data[0], message.data[1], message.data[2], message.data[3],
		message.data[4], message.data[5], message.data[6], message.data[7] );
	
	// Send
	if (canUsb.isReadyToSend())
	{
		if (canUsb.sendMessage(message)) {
			ROS_DEBUG("Send ok");
		} else {
			ROS_ERROR("Sending failed of CAN message with Id 0x%04x", message.getIdentifier());
		}
	}
	else
	{
		ROS_ERROR("Could not send CAN message because canUsb is not ready to send.");
	}
}

/**
 * Simple ROS node that uses xpcc CAN2USB parser.
 * 
 * It can receive CAN messages on the can2usb/tx topic and sends them to the CAN bus.
 * Messages on the CAN bus (from any other node) is received and broadcasted on the can2usb/rx topic.
 *
 */
int
main(int argc, char **argv)
{
    ros::init(argc, argv, "can2usb");
    ros::NodeHandle n("~");
    ros::Publisher  can2usb_pub = n.advertise<rca_xpcc_pkg::Can>("rx", 1000);
    ros::Subscriber can2usb_sub = n.subscribe("tx", 1000, can2usbCallback);
    ros::Rate loop_rate(1000);

    ROS_INFO("Hello CAN2USB");
    
    std::string device_string;
    n.param("device_string", device_string, std::string("/dev/ttyUSB0"));
    
    int serialBaudrate;
    n.param("serial_baudrate", serialBaudrate, int(115000));
    
    int canBitrate;
    n.param("can_bitrate", canBitrate, int(125000));
    
    // Map canBitrate
    xpcc::Can::Bitrate canBitrateEnum;
    switch (canBitrate)
    {
		case   10000:canBitrateEnum = xpcc::Can::kBps10;  ROS_INFO("Setting CAN bitrate to 10kbps"); break;
		case   20000:canBitrateEnum = xpcc::Can::kBps20;  ROS_INFO("Setting CAN bitrate to 20kbps"); break;
		case   50000:canBitrateEnum = xpcc::Can::kBps50;  ROS_INFO("Setting CAN bitrate to 50kbps"); break;
		case  100000:canBitrateEnum = xpcc::Can::kBps100; ROS_INFO("Setting CAN bitrate to 100kbps"); break;
		case  125000:canBitrateEnum = xpcc::Can::kBps125; ROS_INFO("Setting CAN bitrate to 125kbps"); break;
		case  250000:canBitrateEnum = xpcc::Can::kBps250; ROS_INFO("Setting CAN bitrate to 250kbps"); break;
		case  500000:canBitrateEnum = xpcc::Can::kBps500; ROS_INFO("Setting CAN bitrate to 500kbps"); break;
		case 1000000:canBitrateEnum = xpcc::Can::MBps1;   ROS_INFO("Setting CAN bitrate to 1Mbps"); break;
		default:
			ROS_ERROR("CAN bitrate %d is unsupported.", canBitrate);
			ROS_ERROR("Supported CAN bitrates are 10000, 20000, 50000, 100000, 125000, 250000, 500000, 1000000 is supported.");
			ros::shutdown();
	}

	if (!canUsb.open(device_string, serialBaudrate, canBitrateEnum)) {
    	ROS_ERROR_STREAM("Could not open port " << device_string);
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

