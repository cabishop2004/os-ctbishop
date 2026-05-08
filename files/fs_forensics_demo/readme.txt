For this filesystem project, I created a small ext4 filesystem inside a file
called evidence.img. I used debugfs to write three files into the image,
including secret.txt. After confirming that secret.txt existed in the
directory listing, I deleted it using debugfs. When I listed the filesystem
again, secret.txt was gone, which showed that the file had been deleted from
the normal filesystem view.

After that, I searched the raw disk image using strings and grep. Even though
secret.txt was no longer listed, the command strings evidence.img | grep
SECRET_EVIDENCE still found the original text from the deleted file. This
shows that file deletion and data destruction are not the same thing. The
filesystem removed the directory entry, but the actual bytes were still
present in the image until overwritten.

This demonstrates a basic filesystem forensics concept. Deleted data may still
be recoverable from raw storage, which is why forensic tools can sometimes
recover deleted files and why secure deletion requires overwriting data rather
than only deleting the file.



intructions list : 
|

Create a folder for the demo and move into it.

mkdir fs_forensics_demo
cd fs_forensics_demo

Create a fake 64 MB disk image file. This acts like a small virtual drive.

dd if=/dev/zero of=evidence.img bs=1M count=64

Format the image as an ext4 filesystem.

mkfs.ext4 evidence.img

Create three normal text files on the Linux system before putting them into the image.

echo "Normal class notes file." > notes.txt
echo "SECRET_EVIDENCE: The deleted file still left readable data behind." > secret.txt
echo "Another normal file." > todo.txt

Write those files into the ext4 filesystem image using debugfs.

debugfs -w -R "write notes.txt notes.txt" evidence.img
debugfs -w -R "write secret.txt secret.txt" evidence.img
debugfs -w -R "write todo.txt todo.txt" evidence.img

List the files inside the image to prove that secret.txt exists before deletion.

debugfs -R "ls -l" evidence.img

Delete secret.txt from the filesystem image.

debugfs -w -R "rm secret.txt" evidence.img

List the files again to prove that secret.txt is no longer shown in the directory.

debugfs -R "ls -l" evidence.img

Search the raw image for the deleted file’s text. This proves the deleted data is still recoverable.

strings evidence.img | grep SECRET_EVIDENCE