(require-extension bind coops cplusplus-object)
(bind* "#include <Application.h>\n#include <kernel/OS.h>\n"
 "#include \"app.h\"\n#include \"common/aes2501_app_common.h\"")
#|(define spawn_thread (foreign-lambda integer32 "spawn_thread" thread_func
 (const c-string) integer32 c-pointer))|#
(bind "___s32 spawn_thread(thread_func, const char *, ___s32, AEScan *);") ; !
(define resume_thread (foreign-lambda integer32 "resume_thread" integer32))

(define (make-kernel-thread obj)
 (SpawnThreadProxy "AEScan Runner" 50 obj)
)
(define (b-good? ch) (if (>= ch 0) #t #f))
(define (kernel-thread-start! th)
 (if (b-good? th)
  (resume_thread th)
 )
)
; (define instance (force init-val))
(define init-val (delay
 (let ((obj (new <AEScan> kAesSignature)))
  (kernel-thread-start! (make-kernel-thread obj))
  obj
 )
))

;;;

