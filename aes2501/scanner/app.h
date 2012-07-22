#ifndef AESCAN_H
#define AESCAN_H

class AEScan: public BApplication
{
public:
AEScan(const char *sig) : BApplication(sig) {};
virtual void MessageReceived(BMessage*) {};
};

#endif
