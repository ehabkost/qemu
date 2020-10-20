# Test for -no-hpet and -machine hpet=... options
#
#  Copyright (c) 2020 Red Hat Inc
#
# Author:
#  Eduardo Habkost <ehabkost@redhat.com>
#
# This work is licensed under the terms of the GNU GPL, version 2 or
# later.  See the COPYING file in the top-level directory.

import avocado_qemu
from qemu.qtest import QEMUQtestMachine

class PcHpetOption(avocado_qemu.Test):
    """
    :avocado: tags=machine:pc,arch:x86_64
    """
    def get_qtest_vm(self, *args):
        vm = QEMUQtestMachine(self.qemu_bin)
        vm.add_args('-S')
        vm.add_args(*args)
        vm.launch()
        return vm

    def has_hpet_type(self, vm):
        """Return True if hpet device is supported by the QEMU binary"""
        types = vm.command('qom-list-types',
                                implements='hpet', abstract=False)
        return bool(types)

    def has_hpet_dev(self, vm):
        """Return True if hpet device was created by machine"""
        r = vm.qtest('readl 0xfed00000')
        if not r.startswith('OK '):
            return False
        return int(r[3:], 0) != 0

    def hpet_enabled(self, vm):
        return vm.command('qom-get', path='/machine', property='hpet')

    def test_default(self):
        vm = self.get_qtest_vm()
        if self.has_hpet_type(vm):
            self.assertTrue(self.hpet_enabled(vm))
            self.assertTrue(self.has_hpet_dev(vm))
        else:
            self.assertFalse(self.hpet_enabled(vm))
            self.assertFalse(self.has_hpet_dev(vm))

    def test_no_hpet(self):
        vm = self.get_qtest_vm('-no-hpet')
        self.assertFalse(self.hpet_enabled(vm))
        self.assertFalse(self.has_hpet_dev(vm))

    def test_machine_hpet_off(self):
        vm = self.get_qtest_vm('-machine', 'hpet=off')
        self.assertFalse(self.hpet_enabled(vm))
        self.assertFalse(self.has_hpet_dev(vm))

    def test_machine_hpet_on(self):
        with self.get_qtest_vm() as vm:
            if not self.has_hpet_type(vm):
                self.cancel('HPET device not compiled in')

        vm = self.get_qtest_vm('-machine', 'hpet=on')
        self.assertTrue(self.hpet_enabled(vm))
        self.assertTrue(self.has_hpet_dev(vm))
