import {fileURLToPath, URL} from 'node:url'

import {defineConfig} from 'vite'
import vue from '@vitejs/plugin-vue'
import {viteSingleFile} from "vite-plugin-singlefile"
import Icons from 'unplugin-icons/vite'
import Components from 'unplugin-vue-components/vite'
import IconsResolve from 'unplugin-icons/resolver'
import VieI18nPlugin from '@intlify/unplugin-vue-i18n/vite'
import {resolve, dirname} from 'node:path'

// https://vitejs.dev/config/
export default defineConfig({
    plugins: [
        vue(),
        viteSingleFile(),
        Components({
            resolvers: [IconsResolve()],
            dts: true,
        }),
        Icons({
            compiler: 'vue3',
            autoInstall: true,
        }),
        VieI18nPlugin({
            include: resolve(dirname(fileURLToPath(import.meta.url)), './src/i18n/locales/**'),
        })
    ],
    server: {
        proxy: {
            '^/api': {
                target: 'http://192.168.178.201',
            }
        },
    },
    resolve: {
        alias: {
            '@': fileURLToPath(new URL('./src', import.meta.url)),
        }
    }
})
