@@ -61,7 +61,7 @@ static int my_seq_show(struct seq_file *s, void *v)
    return 0;
}

/* This structure gather "function" to manage the sequence */
/* This structure gathers functions to manage the sequence */
static struct seq_operations my_seq_ops = {
    .start = my_seq_start,
    .next = my_seq_next,
@@ -75,7 +75,7 @@ static int my_open(struct inode *inode, struct file *file)
    return seq_open(file, &my_seq_ops);
};

/* This structure gather "function" that manage the /proc file */
/* This structure gathers functions that manage the /proc file */
#ifdef HAVE_PROC_OPS
static const struct proc_ops my_file_ops = {
    .proc_open = my_open,
