#include <include/errno.h>
#include <kernel/system/time.h>
#include <kernel/memory/malloc.h>
#include <kernel/locking/semaphore.h>
#include <kernel/proc/task.h>
#include "pipe.h"

extern process *current_process;

ssize_t pipe_read(vfs_file *file, char *buf, size_t count, loff_t ppos)
{
  if (file->f_flags & O_WRONLY)
    return -EINVAL;

  pipe *p = file->f_dentry->d_inode->i_pipe;
  acquire_semaphore(&p->mutex);
  for (uint32_t i = 0; i < count; ++i)
    circular_buf_get(p->buf, buf + i);
  release_semaphore(&p->mutex);
  return 0;
}

ssize_t pipe_write(vfs_file *file, const char *buf, size_t count, loff_t ppos)
{
  if (file->f_flags & O_RDONLY)
    return -EINVAL;

  pipe *p = file->f_dentry->d_inode->i_pipe;
  acquire_semaphore(&p->mutex);
  for (uint32_t i = 0; i < count; ++i)
    circular_buf_put(p->buf, buf[i]);
  release_semaphore(&p->mutex);
  return 0;
}

int pipe_open(vfs_inode *inode, vfs_file *file)
{
  pipe *p = inode->i_pipe;

  acquire_semaphore(&p->mutex);
  switch (file->f_flags)
  {
  case O_RDONLY:
    p->readers++;
    break;

  case O_WRONLY:
    p->writers++;
    break;
  }
  release_semaphore(&p->mutex);
  return 0;
}

int pipe_release(vfs_inode *inode, vfs_file *file)
{
  pipe *p = inode->i_pipe;

  acquire_semaphore(&p->mutex);
  p->files--;
  switch (file->f_flags)
  {
  case O_RDONLY:
    p->readers--;
    break;

  case O_WRONLY:
    p->writers--;
    break;
  }
  release_semaphore(&p->mutex);

  if (!p->files && !p->writers && !p->readers)
  {
    inode->i_pipe = NULL;
    circular_buf_free(p->buf);
    free(p);
  }
  return 0;
}

vfs_file_operations pipe_fops = {
    .read = pipe_read,
    .write = pipe_write,
    .open = pipe_open,
    .release = pipe_release,
};

pipe *alloc_pipe()
{
  pipe *p = malloc(sizeof(pipe));
  p->files = 0;
  p->readers = 0;
  p->writers = 0;

  sema_init(&p->mutex, 1);

  char *buf = malloc(PIPE_SIZE);
  p->buf = circular_buf_init(buf, PIPE_SIZE);

  return p;
}

vfs_inode *get_pipe_inode()
{
  pipe *p = alloc_pipe();
  p->readers = p->writers = 1;
  p->files = 2;

  vfs_inode *inode = init_inode();

  inode->i_mode = S_IFIFO;
  inode->i_atime.tv_sec = get_seconds(NULL);
  inode->i_ctime.tv_sec = get_seconds(NULL);
  inode->i_mtime.tv_sec = get_seconds(NULL);
  inode->i_pipe = p;
  sema_init(&inode->i_sem, 1);
  inode->i_fop = &pipe_fops;

  return inode;
}

int do_pipe(int *fd)
{
  vfs_inode *inode = get_pipe_inode();
  vfs_dentry *dentry = malloc(sizeof(vfs_dentry));
  dentry->d_inode = inode;

  vfs_file *f1 = malloc(sizeof(vfs_file));
  f1->f_flags = O_RDONLY;
  f1->f_op = &pipe_fops;
  f1->f_dentry = dentry;
  f1->f_count = 1;

  vfs_file *f2 = malloc(sizeof(vfs_file));
  f2->f_flags = O_WRONLY;
  f2->f_op = &pipe_fops;
  f2->f_dentry = dentry;
  f2->f_count = 1;

  int32_t ufd1 = find_unused_fd_slot();
  current_process->files->fd[ufd1] = f1;
  fd[0] = ufd1;

  int32_t ufd2 = find_unused_fd_slot();
  current_process->files->fd[ufd2] = f2;
  fd[1] = ufd2;

  return 0;
}