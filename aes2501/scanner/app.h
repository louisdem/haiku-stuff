#ifndef AESCAN_H
#define AESCAN_H

___abstract class Proto
{
public:
virtual void MessageReceived(BMessage*) = 0;
virtual void ReadyToRun() { be_app->PostMessage(B_QUIT_REQUESTED); };
};
class AEScan: public BApplication
{
public:
AEScan(const char *sig) : BApplication(sig) {};

void _Run() { Run(); };
AEScan *DoQuit() { 
	be_app_messenger.SendMessage(B_QUIT_REQUESTED);
	return this;
};
};

#endif
