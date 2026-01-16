# Vercel Deployment Guide

## Prerequisites
- GitHub account
- Vercel account (sign up at vercel.com)
- Git repository pushed to GitHub

## Deployment Steps

### Option 1: Deploy via Vercel CLI (Recommended)

1. **Install Vercel CLI**
   ```bash
   npm install -g vercel
   ```

2. **Login to Vercel**
   ```bash
   vercel login
   ```

3. **Deploy**
   ```bash
   cd c:\Users\gaura\Desktop\IIT Jodhpur\asthama
   vercel
   ```

4. **Follow prompts:**
   - Set up and deploy? **Y**
   - Which scope? Select your account
   - Link to existing project? **N**
   - Project name? **asthama** (or your preferred name)
   - Directory? **./
   - Override settings? **N**

5. **Production Deployment**
   ```bash
   vercel --prod
   ```

### Option 2: Deploy via Vercel Dashboard

1. **Push to GitHub**
   ```bash
   git add .
   git commit -m "Ready for deployment"
   git push origin main
   ```

2. **Go to Vercel Dashboard**
   - Visit https://vercel.com/new
   - Click "Import Project"
   - Select your GitHub repository

3. **Configure Build Settings**
   - Framework Preset: **Other**
   - Build Command: `quasar build`
   - Output Directory: `dist/spa`
   - Install Command: `npm install`

4. **Deploy**
   - Click "Deploy"
   - Wait for build to complete

## Environment Variables (if needed)

If you need to add environment variables:

1. Go to Project Settings → Environment Variables
2. Add any required variables
3. Redeploy

## Post-Deployment

### Update Firebase Configuration
If using Firebase, add your Vercel domain to:
1. Firebase Console → Authentication → Authorized domains
2. Firebase Console → Hosting (if applicable)

### Test Deployment
1. Visit your Vercel URL
2. Test all features:
   - Dashboard loads
   - Firebase connection works
   - Notifications work
   - Emergency contacts work
   - Test mode works

### Custom Domain (Optional)
1. Go to Project Settings → Domains
2. Add your custom domain
3. Update DNS records as instructed

## Troubleshooting

### Build Fails
- Check build logs in Vercel dashboard
- Ensure all dependencies are in package.json
- Test build locally: `npm run build`

### 404 Errors
- Verify `vercel.json` rewrites are configured
- Check output directory is `dist/spa`

### Firebase Connection Issues
- Add Vercel domain to Firebase authorized domains
- Check Firebase config in code

## Automatic Deployments

Vercel automatically deploys:
- **Production**: Pushes to `main` branch
- **Preview**: Pull requests and other branches

## Commands Reference

```bash
# Deploy to preview
vercel

# Deploy to production
vercel --prod

# Check deployment status
vercel ls

# View logs
vercel logs [deployment-url]

# Remove deployment
vercel rm [deployment-name]
```

## Files Created
- `vercel.json` - Vercel configuration
- This guide - Deployment instructions

## Next Steps
1. Choose deployment method (CLI or Dashboard)
2. Follow steps above
3. Test deployed app
4. Share your live URL!

Your app will be live at: `https://[project-name].vercel.app`
