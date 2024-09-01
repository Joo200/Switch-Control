<!--
  - Copyright © 2024 Johannes Zangl
  -
  - Permission is hereby granted, free of charge, to any person obtaining a copy of this software
  - and associated documentation files (the “Software”), to deal in the Software without
  - restriction, including without limitation the rights to use, copy, modify, merge, publish,
  - distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
  - Software is furnished to do so, subject to the following conditions:
  -
  - The above copyright notice and this permission notice shall be included in all copies or
  - substantial portions of the Software.
  -
  - THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
  - BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  - NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
  - DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  - FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  -->

<template>
  <b-form-group :label="$t('locale.title')">
    <b-form-select :model-value="locale" @change="switchLanguage">
      <b-form-select-option
          v-for="sLocale in supportedLocales"
          :key="`locale-${sLocale}`"
          :selected="locale === sLocale"
          :value="sLocale"
      >
        {{ t(`locale.${sLocale}`) }}
      </b-form-select-option>
    </b-form-select>
  </b-form-group>
</template>

<script setup>
import {useI18n} from 'vue-i18n'
import Tr from "@/i18n/translation.js"
import {BFormGroup} from "bootstrap-vue-next";

const {t, locale} = useI18n()

const supportedLocales = Tr.supportedLanguage
const switchLanguage = async (event) => {
  const newLocale = event.target.value
  await Tr.switchLocale(newLocale)
}
</script>
