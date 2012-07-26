#ifndef AESCAN_H
#define AESCAN_H

// (bind-opaque-type thread_func (function integer32 (c-pointer)))
typedef long (*thread_func)(void *);

___abstract class Proto
{
public:
virtual void MessageReceived(BMessage*) = 0;
virtual void ReadyToRun() { be_app->PostMessage(B_QUIT_REQUESTED); }
};
class AEScan: public BApplication
{
public:
AEScan(const char *sig) : BApplication(sig) {}
~AEScan() { be_app_messenger.SendMessage(B_QUIT_REQUESTED); }
};

// !
static long RunProxy(void *_this) {
        AEScan *scanner = (AEScan *) _this;
        scanner->Lock();
        return scanner->Run();
}
static long SpawnThreadProxy(const char *s, long p, AEScan *o) {
        return spawn_thread(RunProxy /* ! */, s, p, o);
};

#endif
