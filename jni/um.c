/*
 *
 * Android App Self-Uninstall Watchdog
 */
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/inotify.h>

/* 宏定义begin */
//清0宏
#define MEM_ZERO(pDest, destSize) memset(pDest, 0, destSize)

//LOG宏定义
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, "test", __VA_ARGS__)
#define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, "test",__VA_ARGS__)
#define LOG_WARN(...) __android_log_print(ANDROID_LOG_WARN, "test", __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, "test",__VA_ARGS__)

#define EVENT_NUM 12

char *event_str[EVENT_NUM] = { "IN_ACCESS", "IN_MODIFY", "IN_ATTRIB",
		"IN_CLOSE_WRITE", "IN_CLOSE_NOWRITE", "IN_OPEN", "IN_MOVED_FROM",
		"IN_MOVED_TO", "IN_CREATE", "IN_DELETE", "IN_DELETE_SELF",
		"IN_MOVE_SELF" };

void init_daemon(void) {
	// process ID and Session ID
	int pid, sid;
	pid = fork();
	if (pid < 0) {
		//fork 失败 ，退出
		exit(1);
	}

	if (pid > 0) {
		//父进程退出
		exit(0);
	}
	LOG_INFO("init_daemon pid= %d",pid);
	//Change the file mode mask
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	LOG_INFO("init_daemon sid= %d",sid);
	if (sid < 0) {
		exit(1);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		exit(1);
	}
	LOG_INFO("chdir OK");
	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	LOG_INFO("close OK");
	return;
}

void main(int argc, char *argv[]) {

	if (argc < 4) {
		LOG_INFO("usage: %s path url sdkversion\n", argv[0]);
		return ;
	}

	LOG_INFO("argv[0] =  %s", argv[0]);
	LOG_INFO("argv[1] =  %s", argv[1]);
	LOG_INFO("argv[2] =  %s", argv[2]);
	LOG_INFO("argv[3] =  %s", argv[3]);

	const char *localpath = argv[1];
	const char *url = argv[2];
	int sdkversion = atoi(argv[3]);
	init_daemon();
	//初始化log
	LOG_INFO("init OK localpath = %s", localpath);
	LOG_INFO("init OK url = %s", url);
	LOG_INFO("init OK sdkversion = %d", sdkversion);
	//子进程注册目录监听器
	int fileDescriptor = inotify_init();
	if (fileDescriptor < 0) {
		LOG_ERROR("inotify_init failed !!!");
		exit(1);
	}

	int watchDescriptor;

	watchDescriptor = inotify_add_watch(fileDescriptor, localpath, IN_DELETE);
	if (watchDescriptor < 0) {
		LOG_ERROR("inotify_add_watch failed !!!");
		exit(1);
	}
	//开始监听
	LOG_INFO("start observer");

	char buf[BUFSIZ];
	struct inotify_event *event;
	int i;
	int len;
	int nread;
	buf[sizeof(buf) - 1] = 0;
	while ((len = read(fileDescriptor, buf, sizeof(buf) - 1)) > 0) {
		nread = 0;
		while (len > 0) {
			event = (struct inotify_event *) &buf[nread];
			for (i = 0; i < EVENT_NUM; i++) {
				if ((event->mask >> i) & 1) {
					if (event->len > 0) {
						LOG_INFO("%s --- %s --- %s\n", localpath, event->name,
								event_str[i]);
					} else {
						LOG_INFO("%s --- %s --- %s\n", localpath, " ",
								event_str[i]);
					}
				}
			}
			nread = nread + sizeof(struct inotify_event) + event->len;
			len = len - sizeof(struct inotify_event) - event->len;
		}
		if (access(localpath, 0) == 0) {
			LOG_INFO("before %s is exist!", localpath);
		} else {
			LOG_INFO("before %s is not exist!", localpath);
			break;
		}
		//收到子目录删除的通知后，暂停100ms，等卸载动作完成后，判断文件夹是否存在
		usleep(100000);
		//判断监控目录是否存在,存在表示非卸载，不存在表示卸载
		if (access(localpath, 0) == 0) {
			LOG_INFO("after %s is exist!", localpath);
		} else {
			LOG_INFO("after %s is not exist!", localpath);
			break;
		}
	}
	inotify_rm_watch(fileDescriptor, IN_DELETE);
	//目录不存在log
	LOG_INFO("inotify_rm_watch OK!!!!");
	//扩展：可以执行其他shell命令，am(即activity manager)，可以打开某程序、服务，broadcast intent，等等
	if (sdkversion >= 17) {
		LOG_INFO("start >=17 OK");
		//4.2以上的系统由于用户权限管理更严格，需要加上 --user 0
		execlp("am", "am", "start", "--user", "0", "-a",
				"android.intent.action.VIEW", "-d", url, (char *) NULL);

	} else {
		LOG_INFO("start <17 OK");
		execlp("am", "am", "start", "-a", "android.intent.action.VIEW", "-d",
				url, (char *) NULL);
	}
}

