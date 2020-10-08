# Copyright (c) 2020 Red Hat, Inc.
#
# Author:
#  Eduardo Habkost <ehabkost@redhat.com>
#
# This work is licensed under the terms of the GNU GPL, version 2 or
# later.  See the COPYING file in the top-level directory.
import shlex
import subprocess

import avocado
import avocado_qemu


class ObjectOption(avocado_qemu.Test):
    """Check if ``-object`` option behaves as expected"""

    def run(self, cmd, *args, **kwargs):
        cmdstr = ' '.join(shlex.quote(c) for c in cmd)
        self.log.info("Running command: %s", cmdstr)
        return subprocess.run(cmd, universal_newlines=True, *args, **kwargs)

    def get_devices(self):
        out = self.run([self.qemu_bin, '-object', 'help'],
                       check=True, stdout=subprocess.PIPE).stdout
        lines = out.split('\n')
        return [l.strip() for l in lines[1:] if l.strip()]

    @avocado.fail_on(subprocess.CalledProcessError)
    def test_help(self):
        """Check if ``-object ...,help`` behaves as expected"""
        for device in self.get_devices():
            self.run([self.qemu_bin, '-object', '%s,help' % (device)],
                     check=True,
                     stdout=subprocess.DEVNULL)

    @avocado.fail_on(subprocess.CalledProcessError)
    def test_crash(self):
        """Check that QEMU doesn't crash when using ``-object ...``"""
        for device in self.get_devices():
            r = self.run([self.qemu_bin, '-object',
                                '%s,id=obj0' % (device),
                                '-monitor', 'stdio'],
                         input='quit\n',
                         stdout=subprocess.DEVNULL,
                         stderr=subprocess.PIPE)
            if r.returncode not in (0, 1):
                self.log.warn("QEMU stderr: %s", r.stderr)
                self.log.warn("QEMU exit code: %d", r.returncode)
                if r.returncode < 0:
                    self.fail("QEMU crashed")
                else:
                    self.fail("Unexpected exit code")
